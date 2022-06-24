# Load and dump a CAN database in ARXML format.

import re
import logging
import numbers
from decimal import Decimal
from typing import Any, List, Optional
from copy import deepcopy

from xml.etree import ElementTree

from ..signal import Signal, NamedSignalValue
from ..signal import Decimal as SignalDecimal
from ..message import Message
from ..node import Node
from ..bus import Bus
from ..internal_database import InternalDatabase
from ...utils import type_sort_signals, sort_signals_by_start_bit

LOGGER = logging.getLogger(__name__)

class AutosarDatabaseSpecifics(object):
    """This class collects the AUTOSAR specific information of a system

    """
    def __init__(self,
                 arxml_version):
        self._arxml_version = arxml_version

    @property
    def arxml_version(self):
        """The used version of ARXML file format

        Note that due to technical reasons we always return version
        "4.0.0" for AUTOSAR 4.X.
        """
        return self._arxml_version

class AutosarBusSpecifics(object):
    """This class collects the AUTOSAR specific information of a CAN bus

    """
    def __init__(self):
        pass

class AutosarNodeSpecifics(object):
    """This class collects the AUTOSAR specific information of node that
    is attached to a CAN bus

    AUTOSAR calls such nodes "ECU instances"...
    """
    def __init__(self):
        pass

class AutosarMessageSpecifics(object):
    """This class collects all AUTOSAR specific information of a CAN message

    This means useful information about CAN messages which is provided
    by ARXML files, but is specific to AUTOSAR.
    """

    def __init__(self):
        self._pdu_paths = []
        self._is_nm = False
        self._secoc: Optional['AutosarSecOCProperties'] = None
        self._e2e: Optional['AutosarEnd2EndProperties'] = None
        self._signal_group = None

    @property
    def pdu_paths(self):
        """The ARXML paths of all PDUs featured by this message.

        For the vast majority of messages, this list only has a single
        entry. Messages with multiplexers and container frames are
        different, though.
        """
        return self._pdu_paths

    @property
    def is_nm(self):
        """True iff the message is used for network management
        """
        return self._is_nm

    @property
    def is_secured(self):
        """True iff the message integrity is secured using SecOC
        """
        return self._secoc is not None

    @property
    def secoc(self):
        """The properties required to implement secured on-board communication
        """
        return self._secoc

    @property
    def e2e(self) -> Optional['AutosarEnd2EndProperties']:
        """Returns the end-to-end protection properties for the message"""
        return self._e2e

    @e2e.setter
    def e2e(self, value: Optional['AutosarEnd2EndProperties']) -> None:
        self._e2e = value

class AutosarSecOCProperties:
    """This class collects all attributes that are required to implement the
    AUTOSAR secure on-board communication (SecOC) specification.

    Be aware that the AUTOSAR SecOC specification does not cover the
    required cryptographic primitives themselves, just the
    "scaffolding" around them...
    """
    def __init__(self,
                 auth_algorithm_name: Optional[str],
                 freshness_algorithm_name: Optional[str],
                 payload_length: Optional[int],
                 data_id: Optional[int],
                 auth_tx_bit_length: Optional[int],
                 freshness_bit_length: Optional[int],
                 freshness_tx_bit_length: Optional[int],
                 ):

        self._auth_algorithm_name = auth_algorithm_name
        self._freshness_algorithm_name = freshness_algorithm_name

        self._payload_length = payload_length
        self._data_id = data_id

        self._freshness_bit_length = freshness_bit_length
        self._freshness_tx_bit_length = freshness_tx_bit_length
        self._auth_tx_bit_length = auth_tx_bit_length

    @property
    def freshness_algorithm_name(self) -> Optional[str]:
        """The name of the algorithm used for verifying the freshness of a
        message.

        This can be used to prevent replay attacks. Note that the
        algorithms themselves are manufacturer-specific, i.e., AUTOSAR
        does not define *any* freshness schemes.
        """
        return self._freshness_algorithm_name

    @property
    def auth_algorithm_name(self) -> Optional[str]:
        """The name of the algorithm used for authentication

        Note that the algorithms themselves are manufacturer-specific,
        i.e., AUTOSAR does not define *any* cryptographic schemes.
        """
        return self._auth_algorithm_name

    @property
    def payload_length(self) -> Optional[int]:
        """Returns the number of bytes covered by the payload of the secured
        message

        (The full message length is the length of the payload plus the
        size of the security trailer.)
        """
        return self._payload_length

    @property
    def data_id(self) -> Optional[int]:
        """The data ID required for authentication.

        Be aware that this is a different data ID than that required
        for End-To-End protection.
        """
        return self._data_id

    @property
    def freshness_bit_length(self) -> Optional[int]:
        """The number of bits of the full freshness counter.
        """
        return self._freshness_bit_length

    @property
    def freshness_tx_bit_length(self) -> Optional[int]:
        """The number of least-significant bits of the freshness counter that
        is send as part of the secured frame.

        This number is at most as large as the number of bits of
        freshness counter objects.

        """
        return self._freshness_tx_bit_length

    @property
    def auth_tx_bit_length(self) -> Optional[int]:
        """The number of most significant bits of the authenticator object
        send as part of the secured frame

        This is at most the length of the authenicator.
        """
        return self._auth_tx_bit_length


class AutosarEnd2EndProperties:
    """This class collects all attributes that are required to implement
    AUTOSAR-conformant End-to-End protection (CRCs) of messages
    """

    def __init__(self):
        self._category: Optional[str] = None
        self._data_ids: Optional[List[int]] = None
        self._payload_length: int = 0

    @property
    def category(self) -> Optional[str]:
        """The category string of the applicable end-to-end protection
        mechanism

        Note that the contents of these are not specified by the
        AUTOSAR standard.
        """
        return self._category

    @category.setter
    def category(self, value: Optional[str]) -> None:
        self._category = value

    @property
    def data_ids(self) -> Optional[List[int]]:
        """The list of data IDs applicable
        """
        return self._data_ids

    @data_ids.setter
    def data_ids(self, value: Optional[List[int]]) -> None:
        self._data_ids = value

    @property
    def payload_length(self) -> int:
        """The size of the end-to-end protected data in bytes

        This number includes the end-to-end protection signals
        themselves (i.e. the sequence counter and the CRC value)

        """
        return self._payload_length

    @payload_length.setter
    def payload_length(self, value: int) -> None:
        self._payload_length = value

def parse_number_string(in_string, allow_float=False):
    # the input string contains a dot -> floating point value
    if '.' in in_string:
        tmp = float(in_string)

        if not allow_float and tmp != int(tmp):
            raise ValueError('Floating point value specified where integer '
                             'is required')

        # if an integer is required but a .0 floating point value is
        # specified, we accept the input anyway. (this seems to be an
        # ambiguity in the AUTOSAR specification.)
        return tmp if allow_float else int(tmp)

    in_string = in_string.strip()
    if not in_string:
        return 0
    elif in_string[0] == '0' and in_string[1:2].isdigit():
        # interpret strings starting with a 0 as octal because
        # python's int(*, 0) does not for some reason.
        return int(in_string, 8)

    return int(in_string, 0) # autodetect the base

class SystemLoader(object):
    def __init__(self,
                 root:Any,
                 strict:bool,
                 sort_signals:type_sort_signals=sort_signals_by_start_bit):
        self._root = root
        self._strict = strict
        self._sort_signals = sort_signals

        m = re.match(r'^\{(.*)\}AUTOSAR$', self._root.tag)

        if not m:
            raise ValueError(f"No XML namespace specified or illegal root tag "
                             f"name '{self._root.tag}'")

        xml_namespace = m.group(1)
        self.xml_namespace = xml_namespace
        self._xml_namespaces = { 'ns': xml_namespace }

        m = re.match(r'^http://autosar\.org/schema/r(4\.[0-9.]*)$',
                     xml_namespace)

        if m:
            # AUTOSAR 4: For some reason, all AR 4 revisions always
            # use "http://autosar.org/schema/r4.0" as their XML
            # namespace. To find out the exact revision used (i.e.,
            # 4.0, 4.1, 4.2, ...), the "xsi:schemaLocation" attribute
            # of the root tag needs to be examined. Since this is
            # pretty fragile (the used naming scheme has changed
            # during the AR4 journey and with the latest naming scheme
            # there seems to be no programmatic way to associate the
            # schemaLocation with the AR revision), we pretend to
            # always use AR 4.0...
            autosar_version_string = m.group(1)

        else:
            m = re.match(r'^http://autosar\.org/(3\.[0-9.]*)$', xml_namespace)

            if m:
                # AUTOSAR 3
                autosar_version_string = m.group(1)

            else:
                m = re.match(r'^http://autosar\.org/([0-9.]*)\.DAI\.[0-9]$',
                             xml_namespace)

                if m:
                    # Daimler (for some model ranges)
                    autosar_version_string = m.group(1)

                else:
                    raise ValueError(f"Unrecognized AUTOSAR XML namespace "
                                     f"'{xml_namespace}'")

        m = re.match(r'^([0-9]*)(\.[0-9]*)?(\.[0-9]*)?$',
                     autosar_version_string)

        if not m:
            raise ValueError(f"Could not parse AUTOSAR version "
                             f"'{autosar_version_string}'")

        self.autosar_version_major = \
            int(m.group(1))
        self.autosar_version_minor = \
            0 if m.group(2) is None else int(m.group(2)[1:])
        self.autosar_version_patch = \
            0 if m.group(3) is None else int(m.group(3)[1:])

        if self.autosar_version_major != 4 and self.autosar_version_major != 3:
            raise ValueError('This class only supports AUTOSAR '
                             'versions 3 and 4')

        self._create_arxml_reference_dicts()

    def autosar_version_newer(self, major, minor=None, patch=None):
        """Returns true iff the AUTOSAR version specified in the ARXML it at
        least as the version specified by the function parameters

        If a part of the specified version is 'None', it and the
        'lesser' parts of the version are not considered. Also, the
        major version number *must* be specified.
        """

        if self.autosar_version_major > major:
            return True
        elif self.autosar_version_major < major:
            return False

        # the major part of the queried version is identical to the
        # one used by the ARXML
        if minor is None:
            # don't care
            return True
        elif self.autosar_version_minor > minor:
            return True
        elif self.autosar_version_minor < minor:
            return False

        # the major and minor parts of the queried version are identical
        # to the one used by the ARXML
        if patch is None:
            # don't care
            return True
        elif self.autosar_version_patch > patch:
            return True
        elif self.autosar_version_patch < patch:
            return False

        # all parts of the queried version are identical to the one
        # actually used by the ARXML
        return True

    def load(self) -> InternalDatabase:
        messages = []

        if self.autosar_version_newer(4):
            root_packages = self._root.find("./ns:AR-PACKAGES",
                                            self._xml_namespaces)
        else:
            # AUTOSAR3 puts the top level packages beneath the
            # TOP-LEVEL-PACKAGES XML tag.
            root_packages = self._root.find("./ns:TOP-LEVEL-PACKAGES",
                                            self._xml_namespaces)

        buses = self._load_buses(root_packages)
        nodes = self._load_nodes(root_packages)
        messages = self._load_messages(root_packages)

        # the senders and receivers can only be loaded once all
        # messages are known...
        self._load_senders_and_receivers(root_packages, messages)

        # although there must only be one system globally, it can be
        # located within any package and the parameters which it
        # specifies affect a bunch of messages at once. we thus have
        # to load it separately...
        self._load_system(root_packages, messages)

        arxml_version = \
            f'{self.autosar_version_major}.' \
            f'{self.autosar_version_minor}.' \
            f'{self.autosar_version_patch}'

        autosar_specifics = \
            AutosarDatabaseSpecifics(arxml_version=arxml_version)

        # the data IDs (for end-to-end protection)
        self._load_e2e_properties(root_packages, messages)

        return InternalDatabase(buses=buses,
                                nodes=nodes,
                                messages=messages,
                                version=None,
                                autosar_specifics=autosar_specifics)

    def _load_buses(self, package_list):
        """Recursively extract all buses of all CAN clusters of a list of
        AUTOSAR packages.

        @return The list of all buses contained in the given list of
                packages and their sub-packages
        """

        buses = []

        for package in package_list:
            can_clusters = \
                self._get_arxml_children(package,
                                         [
                                             'ELEMENTS',
                                             '*&CAN-CLUSTER',
                                         ])

            # handle locally-specified clusters
            for can_cluster in can_clusters:
                autosar_specifics = AutosarBusSpecifics()

                if self.autosar_version_newer(4):
                    name = \
                        self._get_unique_arxml_child(can_cluster,
                                                     'SHORT-NAME').text
                    comments = self._load_comments(can_cluster)
                    variants = \
                        self._get_arxml_children(can_cluster,
                                                 [
                                                     'CAN-CLUSTER-VARIANTS',
                                                     '*CAN-CLUSTER-CONDITIONAL',
                                                 ])

                    if variants is None or len(variants) == 0:
                        # WTH?
                        continue
                    elif len(variants) > 1:
                        LOGGER.warning(f'Multiple variants specified for CAN '
                                       f'cluster "{name}". Using first one.')

                    variant = variants[0]

                    # version of the CAN standard
                    proto_version = \
                        self._get_unique_arxml_child(variant,
                                                     'PROTOCOL-VERSION')
                    if proto_version is not None:
                        proto_version = proto_version.text

                    # base signaling rate
                    baudrate = self._get_unique_arxml_child(variant, 'BAUDRATE')
                    if baudrate is not None:
                        baudrate = parse_number_string(baudrate.text)

                    # baudrate for the payload of CAN-FD frames. (None if
                    # this bus does not use CAN-FD.)
                    fd_baudrate = \
                        self._get_unique_arxml_child(variant, 'CAN-FD-BAUDRATE')
                    if fd_baudrate is not None:
                        fd_baudrate = parse_number_string(fd_baudrate.text)

                    buses.append(Bus(name=name,
                                     comment=comments,
                                     autosar_specifics=autosar_specifics,
                                     baudrate=baudrate,
                                     fd_baudrate=fd_baudrate))
                else: # AUTOSAR 3
                    name = \
                        self._get_unique_arxml_child(can_cluster,
                                                     'SHORT-NAME').text
                    comments = self._load_comments(can_cluster)

                    # version of the CAN standard
                    proto_version = \
                        self._get_unique_arxml_child(can_cluster,
                                                     'PROTOCOL-VERSION')
                    if proto_version is not None:
                        proto_version = proto_version.text

                    # base signaling rate
                    baudrate = self._get_unique_arxml_child(can_cluster,
                                                            'SPEED')
                    if baudrate is not None:
                        baudrate = parse_number_string(baudrate.text)

                    # AUTOSAR 3 does not seem to support CAN-FD
                    fd_baudrate = None

                    buses.append(Bus(name=name,
                                     comment=comments,
                                     autosar_specifics=autosar_specifics,
                                     baudrate=baudrate,
                                     fd_baudrate=fd_baudrate))

            # handle all sub-packages
            if self.autosar_version_newer(4):
                sub_package_list = package.find('./ns:AR-PACKAGES',
                                                self._xml_namespaces)
            else:
                sub_package_list = package.find('./ns:SUB-PACKAGES',
                                                self._xml_namespaces)

            if sub_package_list is not None:
                buses.extend(self._load_buses(sub_package_list))

        return buses

    # deal with the senders of messages and the receivers of signals
    def _load_senders_and_receivers(self, package_list, messages):
        if package_list is None:
            return

        for package in self._get_arxml_children(package_list, '*AR-PACKAGE'):
            for ecu_instance in self._get_arxml_children(package,
                                                         [
                                                             'ELEMENTS',
                                                             '*ECU-INSTANCE'
                                                         ]):
                self._load_senders_receivers_of_ecu(ecu_instance, messages)

            self._load_senders_receivers_of_nm_pdus(package, messages)

            # handle sub-packages
            if self.autosar_version_newer(4):
                sub_package_list = self._get_unique_arxml_child(package,
                                                                'AR-PACKAGES')
            else: # AUTOSAR 3
                sub_package_list = self._get_unique_arxml_child(package,
                                                                'SUB-PACKAGES')

            self._load_senders_and_receivers(sub_package_list, messages)

    # given a list of Message objects and an reference to a PDU by its absolute ARXML path,
    # return the subset of messages of the list which feature the specified PDU.
    def __get_messages_of_pdu(self, msg_list, pdu_path):
        pdu_messages = \
            [ x for x in msg_list if pdu_path in x.autosar.pdu_paths ]

        # add all messages featured by container frames
        for message in msg_list:
            if message.contained_messages is None:
                continue

            pdu_messages.extend(
                [
                    x for x in message.contained_messages
                          if pdu_path in x.autosar.pdu_paths
                 ])

        if len(pdu_messages) < 1:
            # hm: the data set seems to be inconsistent
            LOGGER.info(f'PDU "{pdu_path}" seems not to be '
                        f'featured by any message')

        return pdu_messages

    def _load_senders_receivers_of_ecu(self, ecu_instance, messages):
        # get the name of the ECU. Note that in cantools, ECUs
        # are called 'nodes' for all intents and purposes...
        ecu_name = \
            self._get_unique_arxml_child(ecu_instance,
                                         'SHORT-NAME').text.strip()


        ####
        # load senders and receivers of "normal" messages
        ####
        if self.autosar_version_newer(4):
            pdu_groups_spec = [
                'ASSOCIATED-COM-I-PDU-GROUP-REFS',
                '*&ASSOCIATED-COM-I-PDU-GROUP'
            ]
        else: # AUTOSAR 3
            pdu_groups_spec = [
                'ASSOCIATED-I-PDU-GROUP-REFS',
                '*&ASSOCIATED-I-PDU-GROUP'
            ]

        for pdu_group in self._get_arxml_children(ecu_instance,
                                                  pdu_groups_spec):
            comm_dir = \
                self._get_unique_arxml_child(pdu_group,
                                             'COMMUNICATION-DIRECTION')
            comm_dir = comm_dir.text

            if self.autosar_version_newer(4):
                pdu_spec = [
                    'I-SIGNAL-I-PDUS',
                    '*I-SIGNAL-I-PDU-REF-CONDITIONAL',
                    '&I-SIGNAL-I-PDU'
                ]
            else: # AUTOSAR 3
                pdu_spec = [
                    'I-PDU-REFS',
                    '*&I-PDU'
                ]

            for pdu in self._get_arxml_children(pdu_group, pdu_spec):
                pdu_path = self._node_to_arxml_path.get(pdu)
                pdu_messages = \
                    self.__get_messages_of_pdu(messages, pdu_path)

                if comm_dir == 'IN':
                    for pdu_message in pdu_messages:
                        for signal in pdu_message.signals:
                            if ecu_name not in signal.receivers:
                                signal.receivers.append(ecu_name)
                elif comm_dir == 'OUT':
                    for pdu_message in pdu_messages:
                        if ecu_name not in pdu_message.senders:
                            pdu_message.senders.append(ecu_name)

    def _load_senders_receivers_of_nm_pdus(self, package, messages):
        ####
        # senders and receivers of network management messages
        ####

        if not self.autosar_version_newer(4):
            # only AUTOSAR4 seems to support specifying senders and
            # receivers of network management PDUs...
            return

        for nm_cluster in self._get_arxml_children(package,
                                                   [
                                                       'ELEMENTS',
                                                       '*NM-CONFIG',
                                                       'NM-CLUSTERS',
                                                       '*CAN-NM-CLUSTER',
                                                   ]):

            nm_node_spec = [
                'NM-NODES',
                '*CAN-NM-NODE'
            ]
            for nm_node in self._get_arxml_children(nm_cluster, nm_node_spec):
                controller_ref = self._get_unique_arxml_child(nm_node,
                                                              'CONTROLLER-REF')

                if controller_ref is None:
                    continue

                controller_ref = controller_ref.text

                # strip away the last element of the reference's path
                # to get the ECU instance corresponding to the network
                # controller. This approach is a bit hacky because it
                # may break down if reference bases are used. (which
                # seems to be very rarely.)
                ecu_ref = '/'.join(controller_ref.split('/')[:-1])
                ecu = self._follow_arxml_reference(
                    base_elem=nm_node,
                    arxml_path=ecu_ref,
                    dest_tag_name='ECU-INSTANCE')

                if ecu is None:
                    continue

                ecu_name = self._get_unique_arxml_child(ecu, 'SHORT-NAME').text

                # deal with receive PDUs
                for rx_pdu in self._get_arxml_children(nm_node,
                                                       [
                                                           'RX-NM-PDU-REFS',
                                                           '*&RX-NM-PDU'
                                                       ]):
                    pdu_path = self._node_to_arxml_path.get(rx_pdu)
                    pdu_messages = self.__get_messages_of_pdu(messages,
                                                              pdu_path)

                    for pdu_message in pdu_messages:
                        for signal in pdu_message.signals:
                            if ecu_name not in signal.receivers:
                                signal.receivers.append(ecu_name)

                # deal with transmit PDUs
                for tx_pdu in self._get_arxml_children(nm_node,
                                                       [
                                                           'TX-NM-PDU-REFS',
                                                           '*&TX-NM-PDU'
                                                       ]):
                    pdu_path = self._node_to_arxml_path.get(tx_pdu)
                    pdu_messages = self.__get_messages_of_pdu(messages,
                                                              pdu_path)

                    for pdu_message in pdu_messages:
                        if ecu_name not in pdu_message.senders:
                            pdu_message.senders.append(ecu_name)

    def _load_system(self, package_list, messages):
        """Internalize the information specified by the system.

        Note that, even though there might at most be a single system
        specified in the file, the package where this is done is not
        mandated, so we have to go through the whole package hierarchy
        for this.
        """

        for package in package_list:
            system = self._get_unique_arxml_child(package,
                                                  [
                                                      'ELEMENTS',
                                                      'SYSTEM'
                                                  ])

            if system is None:
                # handle sub-packages
                if self.autosar_version_newer(4):
                    sub_package_list = package.find('./ns:AR-PACKAGES',
                                                    self._xml_namespaces)

                else:
                    sub_package_list = package.find('./ns:SUB-PACKAGES',
                                                    self._xml_namespaces)

                if sub_package_list is not None:
                    self._load_system(sub_package_list, messages)

                continue

            # set the byte order of all container messages
            container_header_byte_order = \
                self._get_unique_arxml_child(system,
                                            'CONTAINER-I-PDU-HEADER-BYTE-ORDER')

            if container_header_byte_order is not None:
                container_header_byte_order = container_header_byte_order.text
                if container_header_byte_order == 'MOST-SIGNIFICANT-BYTE-LAST':
                    container_header_byte_order = 'little_endian'
                else:
                    container_header_byte_order = 'big_endian'
            else:
                container_header_byte_order = 'big_endian'

            for message in messages:
                if message.is_container:
                    message.header_byte_order = container_header_byte_order

    def _load_nodes(self, package_list):
        """Recursively extract all nodes (ECU-instances in AUTOSAR-speak) of
        all CAN clusters of a list of AUTOSAR packages.

        @return The list of all nodes contained in the given list of
                packages and their sub-packages
        """

        nodes = []

        for package in package_list:
            for ecu in self._get_arxml_children(package,
                                                [
                                                    'ELEMENTS',
                                                    '*ECU-INSTANCE',
                                                ]):
                name = self._get_unique_arxml_child(ecu, "SHORT-NAME").text
                comments = self._load_comments(ecu)
                autosar_specifics = AutosarNodeSpecifics()

                nodes.append(Node(name=name,
                                  comment=comments,
                                  autosar_specifics=autosar_specifics))

            # handle all sub-packages
            if self.autosar_version_newer(4):
                sub_package_list = package.find('./ns:AR-PACKAGES',
                                                self._xml_namespaces)

            else:
                sub_package_list = package.find('./ns:SUB-PACKAGES',
                                                self._xml_namespaces)

            if sub_package_list is not None:
                nodes.extend(self._load_nodes(sub_package_list))


        return nodes

    def _load_e2e_properties(self, package_list, messages):
        """Internalize AUTOSAR end-2-end protection properties required for
        implementing end-to-end protection (CRCs) of messages.

        """

        for package in package_list:

            # specify DIDs via AUTOSAR E2Eprotection sets
            e2e_protections = \
                self._get_arxml_children(package,
                                         [
                                             'ELEMENTS',
                                             '*END-TO-END-PROTECTION-SET',
                                             'END-TO-END-PROTECTIONS',
                                             '*END-TO-END-PROTECTION',
                                         ])

            for e2e_protection in e2e_protections:
                profile = self._get_unique_arxml_child(e2e_protection,
                                                       'END-TO-END-PROFILE')
                if profile is None:
                    continue

                e2e_props = AutosarEnd2EndProperties()

                category = self._get_unique_arxml_child(profile, 'CATEGORY')
                if category is not None:
                    category = category.text
                e2e_props.category = category

                data_id_elems = \
                    self._get_arxml_children(profile,
                                             [
                                                 'DATA-IDS',
                                                 '*DATA-ID'
                                             ])
                data_ids = []
                for data_id_elem in data_id_elems:
                    data_ids.append(parse_number_string(data_id_elem.text))
                e2e_props.data_ids = data_ids

                pdus = self._get_arxml_children(e2e_protection,
                                [
                                    'END-TO-END-PROTECTION-I-SIGNAL-I-PDUS',
                                    '*END-TO-END-PROTECTION-I-SIGNAL-I-PDU',
                                    '&I-SIGNAL-I-PDU',
                                ])
                for pdu in pdus:
                    pdu_path = self._node_to_arxml_path.get(pdu)
                    pdu_messages = \
                        self.__get_messages_of_pdu(messages, pdu_path)

                    for message in pdu_messages:
                        if message.is_container:
                            # containers are never end-to-end protected,
                            # only the contained messages are
                            continue

                        pdu_e2e = deepcopy(e2e_props)
                        if message.autosar.is_secured:
                            pdu_e2e.payload_length = \
                                message.autosar.secoc.payload_length
                        else:
                            pdu_e2e.payload_length = message.length

                        message.autosar.e2e = pdu_e2e

            # load all sub-packages
            if self.autosar_version_newer(4):
                sub_package_list = package.find('./ns:AR-PACKAGES',
                                            self._xml_namespaces)

            else:
                sub_package_list = package.find('./ns:SUB-PACKAGES',
                                                self._xml_namespaces)

            if sub_package_list is not None:
                self._load_e2e_properties(sub_package_list, messages)

    def _load_messages(self, package_list):
        """Recursively extract all messages of all CAN clusters of a list of
        AUTOSAR packages.

        @return The list of all messages contained in the given list of
                packages and their sub-packages
        """

        messages = []

        # load all messages of all packages in an list of XML package elements
        for package in package_list.iterfind('./ns:AR-PACKAGE',
                                             self._xml_namespaces):
            # deal with the messages of the current package
            messages.extend(self._load_package_messages(package))

            # load all sub-packages
            if self.autosar_version_newer(4):
                sub_package_list = package.find('./ns:AR-PACKAGES',
                                            self._xml_namespaces)

            else:
                sub_package_list = package.find('./ns:SUB-PACKAGES',
                                                self._xml_namespaces)

            if sub_package_list is not None:
                messages.extend(self._load_messages(sub_package_list))

        return messages

    def _load_package_messages(self, package_elem):
        """This code extracts the information about CAN clusters of an
        individual AR package
        """

        messages = []

        can_clusters = self._get_arxml_children(package_elem,
                                                [
                                                    'ELEMENTS',
                                                    '*&CAN-CLUSTER',
                                                ])
        for can_cluster in can_clusters:
            bus_name = self._get_unique_arxml_child(can_cluster,
                                                    'SHORT-NAME').text
            if self.autosar_version_newer(4):
                frame_triggerings_spec = \
                    [
                        'CAN-CLUSTER-VARIANTS',
                        '*&CAN-CLUSTER-CONDITIONAL',
                        'PHYSICAL-CHANNELS',
                        '*&CAN-PHYSICAL-CHANNEL',
                        'FRAME-TRIGGERINGS',
                        '*&CAN-FRAME-TRIGGERING'
                    ]

            # AUTOSAR 3
            else:
                frame_triggerings_spec = \
                    [
                        'PHYSICAL-CHANNELS',
                        '*&PHYSICAL-CHANNEL',

                        # ATTENTION! The trailig 'S' here is in purpose:
                        # It appears in the AUTOSAR 3.2 XSD, but it still
                        # seems to be a typo in the spec...
                        'FRAME-TRIGGERINGSS',

                        '*&CAN-FRAME-TRIGGERING'
                    ]

            can_frame_triggerings = \
                self._get_arxml_children(can_cluster, frame_triggerings_spec)

            for can_frame_triggering in can_frame_triggerings:
                messages.append(self._load_message(bus_name,
                                                   can_frame_triggering))

        return messages

    def _load_message(self, bus_name, can_frame_triggering):
        """Load given message and return a message object.
        """

        # Default values.
        cycle_time = None
        senders = []
        autosar_specifics = AutosarMessageSpecifics()

        can_frame = self._get_can_frame(can_frame_triggering)

        # Name, frame id, length, is_extended_frame and comment.
        name = self._load_message_name(can_frame)
        frame_id = self._load_message_frame_id(can_frame_triggering)
        length = self._load_message_length(can_frame)
        is_extended_frame = \
            self._load_message_is_extended_frame(can_frame_triggering)
        comments = self._load_comments(can_frame)

        rx_behavior = \
            self._get_unique_arxml_child(can_frame_triggering,
                                         'CAN-FRAME-RX-BEHAVIOR')
        tx_behavior = \
            self._get_unique_arxml_child(can_frame_triggering,
                                         'CAN-FRAME-TX-BEHAVIOR')
        if rx_behavior is not None and tx_behavior is not None:
            if rx_behavior.text != tx_behavior.text:
                LOGGER.warning(f'Frame "{name}" specifies different receive '
                               f'and send behavior. This is currently '
                               f'unsupported by cantools.')

        is_fd = \
            (rx_behavior is not None and rx_behavior.text == 'CAN-FD') or \
            (tx_behavior is not None and tx_behavior.text == 'CAN-FD')

        # Usually, a CAN message contains only a single PDU, but for
        # things like multiplexed and container messages, this is not
        # the case...
        pdu = self._get_pdu(can_frame)
        assert pdu is not None
        pdu_path = self._get_pdu_path(can_frame)
        autosar_specifics._pdu_paths.append(pdu_path)

        _, \
            _, \
            signals, \
            cycle_time, \
            child_pdu_paths, \
            contained_messages = \
                self._load_pdu(pdu, name, 1)
        autosar_specifics._pdu_paths.extend(child_pdu_paths)
        autosar_specifics._is_nm = \
            (pdu.tag == f'{{{self.xml_namespace}}}NM-PDU')
        is_secured = \
            (pdu.tag == f'{{{self.xml_namespace}}}SECURED-I-PDU')

        self._load_e2e_data_id_from_signal_group(pdu, autosar_specifics)
        if is_secured:
            self._load_secured_properties(name, pdu, signals, autosar_specifics)

        # the bit pattern used to fill in unused bits to avoid
        # undefined behaviour/information leaks
        unused_bit_pattern = \
            self._get_unique_arxml_child(pdu, 'UNUSED-BIT-PATTERN')
        unused_bit_pattern = \
            0xff if unused_bit_pattern is None \
            else parse_number_string(unused_bit_pattern.text)

        return Message(bus_name=bus_name,
                       frame_id=frame_id,
                       is_extended_frame=is_extended_frame,
                       is_fd=is_fd,
                       name=name,
                       length=length,
                       senders=senders,
                       send_type=None,
                       cycle_time=cycle_time,
                       signals=signals,
                       contained_messages=contained_messages,
                       unused_bit_pattern=unused_bit_pattern,
                       comment=comments,
                       autosar_specifics=autosar_specifics,
                       strict=self._strict,
                       sort_signals=self._sort_signals)

    def _load_secured_properties(self,
                                 message_name,
                                 pdu,
                                 signals,
                                 autosar_specifics):
        payload_pdu = \
            self._get_unique_arxml_child(pdu, [ '&PAYLOAD', '&I-PDU' ])

        payload_length = self._get_unique_arxml_child(payload_pdu, 'LENGTH')
        payload_length = parse_number_string(payload_length.text)

        if autosar_specifics.e2e is None:
            # use the data id from the signal group associated with
            # the payload PDU if the secured PDU does not define a
            # group with a data id...
            self._load_e2e_data_id_from_signal_group(payload_pdu,
                                                     autosar_specifics)

        # data specifying the SecOC "footer" of a secured frame
        auth_algo = self._get_unique_arxml_child(pdu, [
            '&AUTHENTICATION-PROPS',
            'SHORT-NAME' ])
        if auth_algo is not None:
            auth_algo = auth_algo.text

        fresh_algo = self._get_unique_arxml_child(pdu, [
            '&FRESHNESS-PROPS',
            'SHORT-NAME' ])
        if fresh_algo is not None:
            fresh_algo = fresh_algo.text

        data_id = self._get_unique_arxml_child(pdu, [
            'SECURE-COMMUNICATION-PROPS',
            'DATA-ID' ])
        if data_id is not None:
            data_id = parse_number_string(data_id.text)

        auth_tx_len = self._get_unique_arxml_child(pdu, [
            '&AUTHENTICATION-PROPS',
            'AUTH-INFO-TX-LENGTH' ])
        if auth_tx_len is not None:
            auth_tx_len = parse_number_string(auth_tx_len.text)

        fresh_len = self._get_unique_arxml_child(pdu, [
            '&FRESHNESS-PROPS',
            'FRESHNESS-VALUE-LENGTH' ])
        if fresh_len is not None:
            fresh_len = parse_number_string(fresh_len.text)

        fresh_tx_len = self._get_unique_arxml_child(pdu, [
            '&FRESHNESS-PROPS',
            'FRESHNESS-VALUE-TX-LENGTH' ])
        if fresh_tx_len is not None:
            fresh_tx_len = parse_number_string(fresh_tx_len.text)

        # add "pseudo signals" for the truncated freshness value and
        # the truncated authenticator
        if fresh_tx_len is not None and fresh_tx_len > 0:
            signals.append(Signal(name=f'{message_name}_Freshness',
                                  start=payload_length*8 + 7,
                                  length=fresh_tx_len,
                                  byte_order='big_endian',
                                  comment=\
                                  {'FOR-ALL':
                                   f'Truncated freshness value for '
                                   f"'{message_name}'"}))
        if auth_tx_len is not None and auth_tx_len > 0:
            n0 = payload_length*8 + (fresh_tx_len//8)*8 + (7-fresh_tx_len%8)
            signals.append(Signal(name=f'{message_name}_Authenticator',
                                  start=n0,
                                  length=auth_tx_len,
                                  byte_order='big_endian',
                                  comment=\
                                  { 'FOR-ALL':
                                    f'Truncated authenticator value for '
                                    f"'{message_name}'"}))

        # note that the length of the authenificator is implicit:
        # e.g., for an MD5 based message authencation code, it would
        # be 128 bits long which algorithm is used is highly
        # manufacturer specific and determined via the authenticator
        # name.
        autosar_specifics._secoc = \
            AutosarSecOCProperties(
                auth_algorithm_name=auth_algo,
                freshness_algorithm_name=fresh_algo,
                payload_length=payload_length,
                data_id=data_id,
                freshness_bit_length=fresh_len,
                freshness_tx_bit_length=fresh_tx_len,
                auth_tx_bit_length=auth_tx_len)


    def _load_pdu(self, pdu, frame_name, next_selector_idx):
        is_secured = pdu.tag == f'{{{self.xml_namespace}}}SECURED-I-PDU'
        is_container = pdu.tag == f'{{{self.xml_namespace}}}CONTAINER-I-PDU'
        is_multiplexed = pdu.tag == f'{{{self.xml_namespace}}}MULTIPLEXED-I-PDU'

        if is_container:
            max_length = self._get_unique_arxml_child(pdu, 'LENGTH')
            max_length = parse_number_string(max_length.text)

            header_type = self._get_unique_arxml_child(pdu, 'HEADER-TYPE')

            if header_type.text != 'SHORT-HEADER':
                LOGGER.warning(f'Only short headers are currently supported '
                               f'for container frames. Frame "{frame_name}" '
                               f'Uses "{header_type.text}"!')
                return \
                    next_selector_idx, \
                    max_length, \
                    [], \
                    None, \
                    [], \
                    None

            contained_pdus = \
                self._get_arxml_children(pdu,
                                         [
                                             'CONTAINED-PDU-TRIGGERING-REFS',
                                             '*&CONTAINED-PDU-TRIGGERING',
                                             '&I-PDU'
                                         ])
            child_pdu_paths = []
            contained_messages = list()
            for contained_pdu in contained_pdus:
                name = \
                    self._get_unique_arxml_child(contained_pdu, 'SHORT-NAME')
                name = name.text

                length = \
                    self._get_unique_arxml_child(contained_pdu, 'LENGTH')
                length = parse_number_string(length.text)

                header_id = \
                    self._get_unique_arxml_child(contained_pdu,
                                                 [
                                                     'CONTAINED-I-PDU-PROPS',
                                                     'HEADER-ID-SHORT-HEADER'
                                                 ])
                header_id = parse_number_string(header_id.text)

                comments = self._load_comments(contained_pdu)

                # the bit pattern used to fill in unused bits to avoid
                # undefined behaviour/informtion leaks
                unused_bit_pattern = \
                    self._get_unique_arxml_child(contained_pdu,
                                                 'UNUSED-BIT-PATTERN')
                unused_bit_pattern = \
                    0xff if unused_bit_pattern is None \
                    else parse_number_string(unused_bit_pattern.text)

                next_selector_idx, \
                    payload_length, \
                    signals, \
                    cycle_time, \
                    contained_pdu_paths, \
                    contained_inner_messages = \
                        self._load_pdu(contained_pdu,
                                       frame_name,
                                       next_selector_idx)

                assert contained_inner_messages is None, \
                    "Nested containers are not supported!"

                contained_pdu_path = self._node_to_arxml_path[contained_pdu]
                contained_pdu_paths.append(contained_pdu_path)
                child_pdu_paths.extend(contained_pdu_paths)

                # create the autosar specifics of the contained_message
                contained_autosar_specifics = AutosarMessageSpecifics()
                contained_autosar_specifics._pdu_paths = contained_pdu_paths
                is_secured = \
                    (contained_pdu.tag ==
                     f'{{{self.xml_namespace}}}SECURED-I-PDU')

                # load the data ID of the PDU via its associated
                # signal group (if it is specified this way)
                self._load_e2e_data_id_from_signal_group(
                    contained_pdu,
                    contained_autosar_specifics)
                if is_secured:
                    self._load_secured_properties(name,
                                                  contained_pdu,
                                                  signals,
                                                  contained_autosar_specifics)

                contained_message = \
                    Message(header_id=header_id,
                            # work-around the hard-coded assumption
                            # that a message must always exhibit a
                            # frame ID
                            frame_id=1,
                            name=name,
                            length=length,
                            cycle_time=cycle_time,
                            signals=signals,
                            unused_bit_pattern=unused_bit_pattern,
                            comment=comments,
                            autosar_specifics=contained_autosar_specifics,
                            sort_signals=self._sort_signals)

                contained_messages.append(contained_message)

            return next_selector_idx, \
                max_length, \
                [], \
                None, \
                child_pdu_paths, \
                contained_messages

        elif is_secured:
            # secured PDUs reference a payload PDU and some
            # authentication and freshness properties. Currently, we
            # ignore everything except for the payload.
            payload_pdu = \
                self._get_unique_arxml_child(pdu, [ '&PAYLOAD', '&I-PDU' ])
            assert payload_pdu is not None, \
                "Secured PDUs must specify a payload PDU!"

            next_selector_idx, \
                payload_length, \
                signals, \
                cycle_time, \
                child_pdu_paths, \
                contained_messages = \
                    self._load_pdu(payload_pdu, frame_name, next_selector_idx)

            payload_pdu_path = self._node_to_arxml_path[payload_pdu]
            child_pdu_paths.append(payload_pdu_path)

            return next_selector_idx, \
                payload_length, \
                signals, \
                cycle_time, \
                child_pdu_paths, \
                contained_messages

        # load all data associated with this PDU.
        signals = []
        child_pdu_paths = []

        byte_length = self._get_unique_arxml_child(pdu, 'LENGTH')
        if byte_length is not None:
            byte_length = parse_number_string(byte_length.text)

        if self.autosar_version_newer(4):
            time_period_location = [
                'I-PDU-TIMING-SPECIFICATIONS',
                'I-PDU-TIMING',
                'TRANSMISSION-MODE-DECLARATION',
                'TRANSMISSION-MODE-TRUE-TIMING',
                'CYCLIC-TIMING',
                'TIME-PERIOD',
                'VALUE',
            ]
        else:
            time_period_location = [
                'I-PDU-TIMING-SPECIFICATION',
                'CYCLIC-TIMING',
                'REPEATING-TIME',
                'VALUE',
            ]

        time_period = \
            self._get_unique_arxml_child(pdu, time_period_location)

        cycle_time = None
        if time_period is not None:
            cycle_time = int(float(time_period.text) * 1000)

        # ordinary non-multiplexed message
        signals = self._load_pdu_signals(pdu)

        if is_multiplexed:
            # multiplexed signals
            pdu_signals, child_pdu_paths = \
                self._load_multiplexed_pdu(pdu, frame_name, next_selector_idx)
            signals.extend(pdu_signals)

        return \
            next_selector_idx, \
            byte_length, \
            signals, \
            cycle_time, \
            child_pdu_paths, \
            None

    def _load_multiplexed_pdu(self, pdu, frame_name, next_selector_idx):
        child_pdu_paths = []

        selector_pos = \
            self._get_unique_arxml_child(pdu, 'SELECTOR-FIELD-START-POSITION')
        selector_pos = parse_number_string(selector_pos.text)

        selector_len = \
            self._get_unique_arxml_child(pdu, 'SELECTOR-FIELD-LENGTH')
        selector_len = parse_number_string(selector_len.text)

        selector_byte_order = \
            self._get_unique_arxml_child(pdu, 'SELECTOR-FIELD-BYTE-ORDER')
        if selector_byte_order is not None:
            if selector_byte_order.text == 'MOST-SIGNIFICANT-BYTE-FIRST':
                selector_byte_order = 'big_endian'
            else:
                assert selector_byte_order.text == 'MOST-SIGNIFICANT-BYTE-LAST'
                selector_byte_order = 'little_endian'
        else:
            selector_byte_order = 'little_endian'

        selector_signal = Signal(
            name=f'{frame_name}_selector{next_selector_idx}',
            start=selector_pos,
            length=selector_len,
            byte_order=selector_byte_order,
            choices={},
            is_multiplexer=True,
        )
        next_selector_idx += 1

        signals = [ selector_signal ]

        if self.autosar_version_newer(4):
            dynpart_spec = [
                                                   'DYNAMIC-PARTS',
                                                   '*DYNAMIC-PART',
                                                   'DYNAMIC-PART-ALTERNATIVES',
                                                   '*DYNAMIC-PART-ALTERNATIVE',
                                               ]
        else:
            dynpart_spec = [
                                                   'DYNAMIC-PART',
                                                   'DYNAMIC-PART-ALTERNATIVES',
                                                   '*DYNAMIC-PART-ALTERNATIVE',
                                               ]

        for dynalt in self._get_arxml_children(pdu, dynpart_spec):
            dynalt_selector_value = \
                self._get_unique_arxml_child(dynalt, 'SELECTOR-FIELD-CODE')
            dynalt_selector_value = parse_number_string(dynalt_selector_value.text)
            dynalt_pdu = self._get_unique_arxml_child(dynalt, '&I-PDU')
            dynalt_pdu_ref = self._get_unique_arxml_child(dynalt, 'I-PDU-REF')
            dynalt_pdu_ref = \
                self._get_absolute_arxml_path(dynalt,
                                              dynalt_pdu_ref.text,
                                              dynalt_pdu_ref.attrib.get('BASE'))
            child_pdu_paths.append(dynalt_pdu_ref)

            next_selector_idx, \
                dynalt_byte_length, \
                dynalt_signals, \
                dynalt_cycle_time, \
                dynalt_child_pdu_paths, \
                _ \
                = self._load_pdu(dynalt_pdu, frame_name, next_selector_idx)
            child_pdu_paths.extend(dynalt_child_pdu_paths)

            is_initial = \
                self._get_unique_arxml_child(dynalt, 'INITIAL-DYNAMIC-PART')
            is_initial = \
                True \
                if is_initial is not None and is_initial.text == 'true' \
                else False
            if is_initial:
                assert selector_signal.initial is None
                selector_signal.initial = dynalt_selector_value

            # remove the selector signal from the dynamic part (because it
            # logically is in the static part, despite the fact that AUTOSAR
            # includes it in every dynamic part)
            dynalt_selector_signals = \
                [ x for x in dynalt_signals if x.start == selector_pos ]
            assert len(dynalt_selector_signals) == 1
            dselsig = dynalt_selector_signals[0]
            assert dselsig.start == selector_pos
            assert dselsig.length == selector_len

            if dynalt_selector_signals[0].choices is not None:
                selector_signal.choices.update(dynalt_selector_signals[0].choices)

            if dynalt_selector_signals[0].invalid is not None:
                # TODO: this may lead to undefined behaviour if
                # multiple PDU define the choices of their selector
                # signals differently (who does this?)
                selector_signal.invalid = dynalt_selector_signals[0].invalid

            dynalt_signals.remove(dynalt_selector_signals[0])

            # copy the non-selector signals into the list of signals
            # for the PDU. TODO: It would be nicer if the hierarchic
            # structure of the message could be preserved, but this
            # would require a major change in the database format.
            for sig in dynalt_signals:
                # if a given signal is not already under the wings of
                # a sub-multiplexer signal, we claim it for ourselfs
                if sig.multiplexer_signal is None:
                    sig.multiplexer_signal = selector_signal.name
                    sig.multiplexer_ids = [ dynalt_selector_value ]

            signals.extend(dynalt_signals)

            # TODO: the cycle time of the multiplexers can be
            # specified indepently of that of the message. how should
            # this be handled?

        if selector_signal.initial in selector_signal.choices:
            selector_signal.initial = \
                selector_signal.choices[selector_signal.initial]

        if not isinstance(selector_signal.invalid, NamedSignalValue) and \
           selector_signal.invalid in selector_signal.choices:
            selector_signal.invalid = \
                selector_signal.choices[selector_signal.invalid]

        # the static part of the multiplexed PDU
        if self.autosar_version_newer(4):
            static_pdu_refs_spec = [
                'STATIC-PARTS',
                '*STATIC-PART',
                'I-PDU-REF',
            ]
        else:
            static_pdu_refs_spec = [
                'STATIC-PART',
                'I-PDU-REF',
            ]

        for static_pdu_ref in self._get_arxml_children(pdu,
                                                       static_pdu_refs_spec):
            static_pdu_path = \
                self._get_absolute_arxml_path(pdu,
                                              static_pdu_ref.text,
                                              static_pdu_ref.attrib.get('BASE'))
            child_pdu_paths.append(static_pdu_path)

            static_pdu = self._follow_arxml_reference(
                base_elem=pdu,
                arxml_path=static_pdu_path,
                dest_tag_name=static_pdu_ref.attrib.get('DEST'))

            next_selector_idx, \
                byte_length, \
                static_signals, \
                _, \
                static_child_pdu_paths, \
                _, \
                = self._load_pdu(static_pdu, frame_name, next_selector_idx)

            child_pdu_paths.extend(static_child_pdu_paths)
            signals.extend(static_signals)

        return signals, child_pdu_paths

    def _load_pdu_signals(self, pdu):
        signals = []

        if self.autosar_version_newer(4):
            # in AR4, "normal" PDUs use I-SIGNAL-TO-PDU-MAPPINGS whilst network
            # management PDUs use I-SIGNAL-TO-I-PDU-MAPPINGS
            i_signal_to_i_pdu_mappings = \
                self._get_arxml_children(pdu,
                                         [
                                             'I-SIGNAL-TO-PDU-MAPPINGS',
                                             '*&I-SIGNAL-TO-I-PDU-MAPPING'
                                         ])
            i_signal_to_i_pdu_mappings.extend(
                self._get_arxml_children(pdu,
                                         [
                                             'I-SIGNAL-TO-I-PDU-MAPPINGS',
                                             '*&I-SIGNAL-TO-I-PDU-MAPPING'
                                         ]))
        else:
            # in AR3, "normal" PDUs use SIGNAL-TO-PDU-MAPPINGS whilst network
            # management PDUs use I-SIGNAL-TO-I-PDU-MAPPINGS
            i_signal_to_i_pdu_mappings = \
                self._get_arxml_children(pdu,
                                         [
                                             'SIGNAL-TO-PDU-MAPPINGS',
                                             '*&I-SIGNAL-TO-I-PDU-MAPPING'
                                         ])

            i_signal_to_i_pdu_mappings.extend(
                self._get_arxml_children(pdu,
                                         [
                                             'I-SIGNAL-TO-I-PDU-MAPPINGS',
                                             '*&I-SIGNAL-TO-I-PDU-MAPPING'
                                         ]))

        for i_signal_to_i_pdu_mapping in i_signal_to_i_pdu_mappings:
            signal = self._load_signal(i_signal_to_i_pdu_mapping)

            if signal is not None:
                signals.append(signal)

        return signals

    def _load_message_name(self, can_frame_triggering):
        return self._get_unique_arxml_child(can_frame_triggering,
                                            'SHORT-NAME').text

    def _load_message_frame_id(self, can_frame_triggering):
        return parse_number_string(
            self._get_unique_arxml_child(can_frame_triggering,
                                         'IDENTIFIER').text)

    def _load_message_length(self, can_frame):
        return parse_number_string(
            self._get_unique_arxml_child(can_frame,
                                         'FRAME-LENGTH').text)

    def _load_message_is_extended_frame(self, can_frame_triggering):
        can_addressing_mode = \
            self._get_unique_arxml_child(can_frame_triggering,
                                         'CAN-ADDRESSING-MODE')

        return False if can_addressing_mode is None \
                     else can_addressing_mode.text == 'EXTENDED'

    def _load_comments(self, node):
        result = {}

        for l_2 in self._get_arxml_children(node, ['DESC', '*L-2']):
            lang = l_2.attrib.get('L', 'EN')
            result[lang] = l_2.text

        if len(result) == 0:
            return None

        return result

    def _load_e2e_data_id_from_signal_group(self,
                                            pdu,
                                            autosar_specifics):

        pdu_length = self._get_unique_arxml_child(pdu, 'LENGTH')
        pdu_length = parse_number_string(pdu_length.text)

        # the signal group associated with this message
        signal_group = \
            self._get_arxml_children(pdu,
                                     [
                                         'I-SIGNAL-TO-PDU-MAPPINGS',
                                         '*I-SIGNAL-TO-I-PDU-MAPPING',
                                         '&I-SIGNAL-GROUP',
                                     ])

        if len(signal_group) == 0:
            return
        elif len(signal_group) > 1:
            #raise ValueError(f'Multiple signal groups specified for '
            #                 f'pdu "{pdu_name}"')
            pass
        signal_group = signal_group[-1]

        trans_props = self._get_unique_arxml_child(signal_group, [
                'TRANSFORMATION-I-SIGNAL-PROPSS',
                'END-TO-END-TRANSFORMATION-I-SIGNAL-PROPS',
                'END-TO-END-TRANSFORMATION-I-SIGNAL-PROPS-VARIANTS',
                'END-TO-END-TRANSFORMATION-I-SIGNAL-PROPS-CONDITIONAL',
            ])

        if trans_props is None:
            return

        profile_name_elem = self._get_unique_arxml_child(trans_props, [
            '&TRANSFORMER',
            'TRANSFORMATION-DESCRIPTIONS',
            'END-TO-END-TRANSFORMATION-DESCRIPTION',
            'PROFILE-NAME',])

        category = None
        if profile_name_elem is not None:
            category = profile_name_elem.text

        did_elems = self._get_arxml_children(trans_props, [
                'DATA-IDS',
                '*DATA-ID'])
        data_ids = []
        for did_elem in did_elems:
            data_ids.append(parse_number_string(did_elem.text))

        e2e_props = AutosarEnd2EndProperties()
        e2e_props.category = category
        e2e_props.data_ids = data_ids
        e2e_props.payload_length = pdu_length
        autosar_specifics.e2e = e2e_props

    def _load_signal(self, i_signal_to_i_pdu_mapping):
        """Load given signal and return a signal object.

        """
        i_signal = self._get_i_signal(i_signal_to_i_pdu_mapping)

        if i_signal is None:
            # No I-SIGNAL found, i.e. this i-signal-to-i-pdu-mapping is
            # probably a i-signal group. According to the XSD, I-SIGNAL and
            # I-SIGNAL-GROUP-REF are mutually exclusive...
            return None

        # Get the system signal XML node. This may also be a system signal
        # group, in which case we have ignore it if the XSD is to be believed.
        # ARXML is great!
        system_signal = self._get_unique_arxml_child(i_signal, '&SYSTEM-SIGNAL')

        if system_signal is not None \
           and system_signal.tag != f'{{{self.xml_namespace}}}SYSTEM-SIGNAL':
            return None

        # Default values.
        initial = None
        invalid = None
        minimum = None
        maximum = None
        factor = 1
        offset = 0
        unit = None
        choices = None
        comments = None
        receivers = []
        decimal = SignalDecimal(Decimal(factor), Decimal(offset))

        if self.autosar_version_newer(4):
            i_signal_spec = '&I-SIGNAL'
        else:
            i_signal_spec = '&SIGNAL'

        i_signal = self._get_unique_arxml_child(i_signal_to_i_pdu_mapping,
                                                i_signal_spec)
        # Name, start position, length and byte order.
        name = self._load_signal_name(i_signal)
        start_position = \
            self._load_signal_start_position(i_signal_to_i_pdu_mapping)
        length = self._load_signal_length(i_signal, system_signal)
        byte_order = self._load_signal_byte_order(i_signal_to_i_pdu_mapping)

        # Type.
        is_signed, is_float = self._load_signal_type(i_signal)

        if system_signal is not None:
            # Minimum, maximum, factor, offset and choices.
            minimum, maximum, factor, offset, choices, unit, comments = \
                self._load_system_signal(system_signal, decimal, is_float)

        # loading initial values is way too complicated, so it is the
        # job of a separate method
        initial = self._load_arxml_init_value_string(i_signal, system_signal)

        if initial is not None:
            initial_int = None
            try:
                initial_int = parse_number_string(initial)
            except:
                pass

            if choices is not None and initial_int in choices:
                initial = choices[initial_int]
            elif is_float:
                initial = float(initial_int)*factor + offset
            elif initial.strip().lower() == 'true':
                initial = True
            elif initial.strip().lower() == 'false':
                initial = False
            # TODO: strings?
            else:
                initial = initial_int*factor + offset

        invalid = self._load_arxml_invalid_int_value(i_signal, system_signal)

        if invalid is not None:
            if choices is not None and invalid in choices:
                invalid = choices[invalid]
            elif not isinstance(invalid, bool) and \
                 isinstance(invalid, numbers.Number):
                invalid = invalid*factor + offset

        # ToDo: receivers

        return Signal(name=name,
                      start=start_position,
                      length=length,
                      receivers=receivers,
                      byte_order=byte_order,
                      is_signed=is_signed,
                      scale=factor,
                      offset=offset,
                      initial=initial,
                      invalid=invalid,
                      minimum=minimum,
                      maximum=maximum,
                      unit=unit,
                      choices=choices,
                      comment=comments,
                      is_float=is_float,
                      decimal=decimal)

    def _load_signal_name(self, i_signal):
        return self._get_unique_arxml_child(i_signal,
                                            'SHORT-NAME').text

    def _load_signal_start_position(self, i_signal_to_i_pdu_mapping):
        pos = self._get_unique_arxml_child(i_signal_to_i_pdu_mapping,
                                           'START-POSITION').text
        return parse_number_string(pos)

    def _load_signal_length(self, i_signal, system_signal):
        i_signal_length = self._get_unique_arxml_child(i_signal, 'LENGTH')

        if i_signal_length is not None:
            return parse_number_string(i_signal_length.text)

        if not self.autosar_version_newer(4) and system_signal is not None:
            # AUTOSAR3 supports specifying the signal length via the
            # system signal. (AR4 does not.)
            system_signal_length = \
                self._get_unique_arxml_child(system_signal, 'LENGTH')

            if system_signal_length is not None:
                # get the length from the system signal.
                return parse_number_string(system_signal_length.text)

        return None # error?!

    def _load_arxml_init_value_string(self, i_signal, system_signal):
        """"Load the initial value of a signal

        Supported mechanisms are references to constants and direct
        specifcation of the value. Note that this method returns a
        string which must be converted into the signal's data type by
        the calling code.
        """

        # AUTOSAR3 specifies the signal's initial value via
        # the system signal via the i-signal...
        if self.autosar_version_newer(4):
            if i_signal is None:
                return None

            return self._load_arxml_init_value_string_helper(i_signal)
        else:
            if system_signal is None:
                return None

            return self._load_arxml_init_value_string_helper(system_signal)

    def _load_arxml_invalid_int_value(self, i_signal, system_signal):
        """Load a signal's internal value which indicates that it is not valid

        i.e., this returns the value which is transferred over the bus
        before scaling and resolving the named choices. We currently
        only support boolean and integer literals, any other value
        specification will be ignored.
        """

        if self.autosar_version_newer(4):
            invalid_val = \
                self._get_unique_arxml_child(i_signal,
                                             [
                                                 'NETWORK-REPRESENTATION-PROPS',
                                                 'SW-DATA-DEF-PROPS-VARIANTS',
                                                 'SW-DATA-DEF-PROPS-CONDITIONAL',
                                                 'INVALID-VALUE',
                                                 'NUMERICAL-VALUE-SPECIFICATION',
                                                 'VALUE',
                                             ])

            if invalid_val is None:
                return None

            return parse_number_string(invalid_val.text)

        else:
            invalid_val = \
                self._get_unique_arxml_child(system_signal,
                                             [
                                                 '&DATA-TYPE',
                                                 'SW-DATA-DEF-PROPS',
                                                 'INVALID-VALUE'
                                             ])

            if invalid_val is None:
                return None

            literal = self._get_unique_arxml_child(invalid_val,
                                                   [
                                                       'INTEGER-LITERAL',
                                                       'VALUE',
                                                   ])
            if literal is not None:
                return parse_number_string(literal.text)

            literal = self._get_unique_arxml_child(invalid_val,
                                                   [
                                                       'BOOLEAN-LITERAL',
                                                       'VALUE',
                                                   ])
            if literal is not None:
                return literal.text.lower().strip() == 'true'

            return None

    def _load_arxml_init_value_string_helper(self, signal_elem):
        """"Helper function for loading thge initial value of a signal

        This function avoids code duplication between loading the
        initial signal value from the ISignal and the
        SystemSignal. (The latter is only supported by AUTOSAR 3.)
        """
        if self.autosar_version_newer(4):
            value_elem = \
                self._get_unique_arxml_child(signal_elem,
                                             [
                                                'INIT-VALUE',
                                                'NUMERICAL-VALUE-SPECIFICATION',
                                                'VALUE'
                                             ])

            if value_elem is not None:
                # initial value is specified directly.
                return value_elem.text

            value_elem = \
                self._get_unique_arxml_child(signal_elem,
                                             [
                                                'INIT-VALUE',
                                                'CONSTANT-REFERENCE',
                                                '&CONSTANT',
                                                'VALUE-SPEC',
                                                'NUMERICAL-VALUE-SPECIFICATION',
                                                'VALUE'
                                             ])

            if value_elem is not None:
                # initial value is specified via a reference to a constant.
                return value_elem.text

            # no initial value specified or specified in a way which we
            # don't recognize
            return None

        else:
            # AUTOSAR3: AR3 seems to specify initial values by means
            # of INIT-VALUE-REF elements. Unfortunately, these are not
            # standard references so we have to go down a separate
            # code path...
            ref_elem = signal_elem.find(f'./ns:INIT-VALUE-REF',
                                        self._xml_namespaces)

            if ref_elem is None:
                # no initial value found here
                return None

            literal_spec = \
                self._follow_arxml_reference(
                    base_elem=signal_elem,
                    arxml_path=ref_elem.text,
                    dest_tag_name=ref_elem.attrib.get('DEST'),
                    refbase_name=ref_elem.attrib.get('BASE'))
            if literal_spec is None:
                # dangling reference...
                return None

            literal_value = \
                literal_spec.find(f'./ns:VALUE', self._xml_namespaces)
            return None if literal_value is None else literal_value.text

    def _load_signal_byte_order(self, i_signal_to_i_pdu_mapping):
        packing_byte_order = \
            self._get_unique_arxml_child(i_signal_to_i_pdu_mapping,
                                         'PACKING-BYTE-ORDER')

        if packing_byte_order is not None \
           and packing_byte_order.text == 'MOST-SIGNIFICANT-BYTE-FIRST':
            return 'big_endian'
        else:
            return 'little_endian'

    def _load_system_signal_unit(self, system_signal, compu_method):
        res = self._get_unique_arxml_child(system_signal,
                                           [
                                               'PHYSICAL-PROPS',
                                               'SW-DATA-DEF-PROPS-VARIANTS',
                                               '&SW-DATA-DEF-PROPS-CONDITIONAL',
                                               '&UNIT',
                                               'DISPLAY-NAME'
                                           ])

        if res is None and compu_method is not None:
            # try to go via the compu_method
            res = self._get_unique_arxml_child(compu_method,
                                               [
                                                   '&UNIT',
                                                   'DISPLAY-NAME'
                                               ])

        ignorelist = ( "NoUnit", )

        if res is None or res.text in ignorelist:
            return None
        return res.text

    def _load_texttable(self, compu_method, decimal, is_float):
        minimum = None
        maximum = None
        choices = {}

        for compu_scale in self._get_arxml_children(compu_method,
                                                    [
                                                      '&COMPU-INTERNAL-TO-PHYS',
                                                      'COMPU-SCALES',
                                                      '*&COMPU-SCALE'
                                                    ]):
            lower_limit = \
                self._get_unique_arxml_child(compu_scale, 'LOWER-LIMIT')
            upper_limit = \
                self._get_unique_arxml_child(compu_scale, 'UPPER-LIMIT')
            vt = \
               self._get_unique_arxml_child(compu_scale, ['&COMPU-CONST', 'VT'])
            comments = self._load_comments(compu_scale)

            # range of the internal values of the scale.
            minimum_int_scale = \
               None \
               if lower_limit is None \
               else parse_number_string(lower_limit.text, is_float)
            maximum_int_scale = \
               None \
               if upper_limit is None \
               else parse_number_string(upper_limit.text, is_float)

            # for texttables the internal and the physical values are identical
            if minimum is None:
                minimum = minimum_int_scale
            elif minimum_int_scale is not None:
                minimum = min(minimum, minimum_int_scale)

            if maximum is None:
                maximum = maximum_int_scale
            elif maximum_int_scale is not None:
                maximum = max(maximum, maximum_int_scale)

            if vt is not None:
                value = parse_number_string(lower_limit.text, is_float)
                name = vt.text
                choices[value] = NamedSignalValue(value, name, comments)

        decimal.minimum = minimum
        decimal.maximum = maximum
        return minimum, maximum, choices

    def _load_linear_factor_and_offset(self, compu_scale, decimal):
        compu_rational_coeffs = \
            self._get_unique_arxml_child(compu_scale, '&COMPU-RATIONAL-COEFFS')

        if compu_rational_coeffs is None:
            return None, None

        numerators = self._get_arxml_children(compu_rational_coeffs,
                                              ['&COMPU-NUMERATOR', '*&V'])

        if len(numerators) != 2:
            raise ValueError(
                'Expected 2 numerator values for linear scaling, but '
                'got {}.'.format(len(numerators)))

        denominators = self._get_arxml_children(compu_rational_coeffs,
                                                ['&COMPU-DENOMINATOR', '*&V'])

        if len(denominators) != 1:
            raise ValueError(
                'Expected 1 denominator value for linear scaling, but '
                'got {}.'.format(len(denominators)))

        denominator = Decimal(denominators[0].text)
        decimal.scale = Decimal(numerators[1].text) / denominator
        decimal.offset = Decimal(numerators[0].text) / denominator

        return float(decimal.scale), float(decimal.offset)

    def _load_linear(self, compu_method, decimal, is_float):
        compu_scale = self._get_unique_arxml_child(compu_method,
                                                   [
                                                       'COMPU-INTERNAL-TO-PHYS',
                                                       'COMPU-SCALES',
                                                       '&COMPU-SCALE'
                                                   ])

        lower_limit = self._get_unique_arxml_child(compu_scale, '&LOWER-LIMIT')
        upper_limit = self._get_unique_arxml_child(compu_scale, '&UPPER-LIMIT')

        # range of the internal values
        minimum_int = \
            None \
            if lower_limit is None \
            else parse_number_string(lower_limit.text, is_float)
        maximum_int = \
            None \
            if upper_limit is None \
            else parse_number_string(upper_limit.text, is_float)

        factor, offset = \
            self._load_linear_factor_and_offset(compu_scale, decimal)

        factor = 1.0 if factor is None else factor
        offset = 0.0 if offset is None else offset

        # range of the physical values
        minimum = None if minimum_int is None else minimum_int*factor + offset
        maximum = None if maximum_int is None else maximum_int*factor + offset
        decimal.minimum = None if minimum is None else Decimal(minimum)
        decimal.maximum = None if maximum is None else Decimal(maximum)

        return minimum, maximum, factor, offset

    def _load_scale_linear_and_texttable(self, compu_method, decimal, is_float):
        minimum = None
        maximum = None
        factor = 1
        offset = 0
        choices = {}

        for compu_scale in self._get_arxml_children(compu_method,
                                                    [
                                                      '&COMPU-INTERNAL-TO-PHYS',
                                                      'COMPU-SCALES',
                                                      '*&COMPU-SCALE'
                                                    ]):

            lower_limit = \
                self._get_unique_arxml_child(compu_scale, 'LOWER-LIMIT')
            upper_limit = \
                self._get_unique_arxml_child(compu_scale, 'UPPER-LIMIT')
            vt = \
               self._get_unique_arxml_child(compu_scale, ['&COMPU-CONST', 'VT'])
            comments = self._load_comments(compu_scale)

            # range of the internal values of the scale.
            minimum_int_scale = \
                None if lower_limit is None \
                else parse_number_string(lower_limit.text, is_float)
            maximum_int_scale = \
               None if upper_limit is None \
               else parse_number_string(upper_limit.text, is_float)

            # TODO: make sure that no conflicting scaling factors and offsets
            # are specified. For now, let's just assume that the ARXML file is
            # well formed.
            factor_scale, offset_scale = \
                self._load_linear_factor_and_offset(compu_scale, decimal)
            if factor_scale is not None:
                factor = factor_scale
            else:
                factor_scale = 1.0

            if offset_scale is not None:
                offset = offset_scale
            else:
                offset_scale = 0.0

            # range of the physical values of the scale.
            if minimum is None:
                minimum = minimum_int_scale*factor_scale + offset_scale
            elif minimum_int_scale is not None:
                minimum = min(minimum,
                              minimum_int_scale*factor_scale + offset_scale)

            if maximum is None:
                maximum = maximum_int_scale*factor_scale + offset_scale
            elif maximum_int_scale is not None:
                maximum = max(maximum,
                              maximum_int_scale*factor_scale + offset_scale)

            if vt is not None:
                assert(minimum_int_scale is not None \
                       and minimum_int_scale == maximum_int_scale)
                value = minimum_int_scale
                name = vt.text
                choices[value] = NamedSignalValue(value, name, comments)

        decimal.minimum = Decimal(minimum)
        decimal.maximum = Decimal(maximum)
        return minimum, maximum, factor, offset, choices

    def _load_system_signal(self, system_signal, decimal, is_float):
        minimum = None
        maximum = None
        factor = 1
        offset = 0
        choices = None

        compu_method = self._get_compu_method(system_signal)

        # Unit and comment.
        unit = self._load_system_signal_unit(system_signal, compu_method)
        comments = self._load_comments(system_signal)

        if compu_method is not None:
            category = self._get_unique_arxml_child(compu_method, 'CATEGORY')

            if category is None:
                # if no category is specified, we assume that the
                # physical value of the signal corresponds to its
                # binary representation.
                return (minimum,
                        maximum,
                        factor,
                        offset,
                        choices,
                        unit,
                        comments)

            category = category.text

            if category == 'TEXTTABLE':
                minimum, maximum, choices = \
                    self._load_texttable(compu_method, decimal,  is_float)
            elif category == 'LINEAR':
                minimum, maximum, factor, offset = \
                    self._load_linear(compu_method, decimal,  is_float)
            elif category == 'SCALE_LINEAR_AND_TEXTTABLE':
                (minimum,
                 maximum,
                 factor,
                 offset,
                 choices) = self._load_scale_linear_and_texttable(compu_method,
                                                                  decimal,
                                                                  is_float)
            else:
                LOGGER.debug('Compu method category %s is not yet implemented.',
                             category)

        return \
            minimum, \
            maximum, \
            1 if factor is None else factor, \
            0 if offset is None else offset, \
            choices, \
            unit, \
            comments

    def _load_signal_type(self, i_signal):
        is_signed = False
        is_float = False

        base_type = self._get_sw_base_type(i_signal)

        if base_type is not None:
            base_type_encoding = \
                self._get_unique_arxml_child(base_type, '&BASE-TYPE-ENCODING')

            if base_type_encoding is None:
                btt = base_type.find('./ns:SHORT-NAME', self._xml_namespaces)
                btt = btt.text
                raise ValueError(
                    f'BASE-TYPE-ENCODING in base type "{btt}" does not exist.')

            base_type_encoding = base_type_encoding.text

            if base_type_encoding in ('2C', '1C', 'SM'):
                # types which use two-complement, one-complement or
                # sign+magnitude encodings are signed. TODO (?): The
                # fact that if anything other than two complement
                # notation is used for negative numbers is not
                # reflected anywhere. In practice this should not
                # matter, though, since two-complement notation is
                # basically always used for systems build after
                # ~1970...
                is_signed = True
            elif base_type_encoding == 'IEEE754':
                is_float = True

        return is_signed, is_float

    def _get_absolute_arxml_path(self,
                                 base_elem,
                                 arxml_path,
                                 refbase_name=None):
        """Return the absolute ARXML path of a reference

        Relative ARXML paths are converted into absolute ones.
        """

        if arxml_path.startswith('/'):
            # path is already absolute
            return arxml_path

        base_path = self._node_to_arxml_path[base_elem]
        base_path_atoms = base_path.split("/")

        # Find the absolute path specified by the applicable
        # reference base. The spec says the matching reference
        # base for the "closest" package should be used, so we
        # traverse the ARXML path of the base element in reverse
        # to find the first package with a matching reference
        # base.
        refbase_path = None
        for i in range(len(base_path_atoms), 0, -1):
            test_path = '/'.join(base_path_atoms[0:i])
            test_node = self._arxml_path_to_node.get(test_path)
            if test_node is not None \
               and test_node.tag  != f'{{{self.xml_namespace}}}AR-PACKAGE':
                # the referenced XML node does not represent a
                # package
                continue

            if refbase_name is None:
                # the caller did not specify a BASE attribute,
                # i.e., we ought to use the closest default
                # reference base
                refbase_path = \
                    self._package_default_refbase_path.get(test_path)
                if refbase_path is None:
                    # bad luck: this package does not specify a
                    # default reference base
                    continue
                else:
                    break

            # the caller specifies a BASE attribute
            refbase_path = \
                self._package_refbase_paths.get(test_path, {}) \
                                           .get(refbase_name)
            if refbase_path is None:
                # bad luck: this package does not specify a
                # reference base with the specified name
                continue
            else:
                break

        if refbase_path is None:
            raise ValueError(f"Unknown reference base '{refbase_name}' "
                             f"for relative ARXML reference '{arxml_path}'")

        return f'{refbase_path}/{arxml_path}'

    def _follow_arxml_reference(self,
                                base_elem,
                                arxml_path,
                                dest_tag_name=None,
                                refbase_name=None):
        """Resolve an ARXML reference

        It returns the ElementTree node which corresponds to the given
        path through the ARXML package structure. If no such node
        exists, a None object is returned.
        """

        arxml_path = self._get_absolute_arxml_path(base_elem,
                                                   arxml_path,
                                                   refbase_name)


        # resolve the absolute reference: This is simple because we
        # have a path -> XML node dictionary!
        result = self._arxml_path_to_node.get(arxml_path)

        if result is not None \
           and dest_tag_name is not None \
           and result.tag != f'{{{self.xml_namespace}}}{dest_tag_name}':
            # the reference could be resolved but it lead to a node of
            # unexpected kind
            return None

        return result


    def _create_arxml_reference_dicts(self):
        self._node_to_arxml_path = {}
        self._arxml_path_to_node = {}
        self._package_default_refbase_path = {}
        # given a package name, produce a refbase label to ARXML path dictionary
        self._package_refbase_paths = {}

        def add_sub_references(elem, elem_path, cur_package_path=""):
            """Recursively add all ARXML references contained within an XML
            element to the dictionaries to handle ARXML references"""

            # check if a short name has been attached to the current
            # element. If yes update the ARXML path for this element
            # and its children
            short_name = elem.find(f'ns:SHORT-NAME', self._xml_namespaces)

            if short_name is not None:
                short_name = short_name.text
                elem_path = f'{elem_path}/{short_name}'

                if elem_path in self._arxml_path_to_node:
                    raise ValueError(f"File contains multiple elements with "
                                     f"path '{elem_path}'")

                self._arxml_path_to_node[elem_path] = elem

            # register the ARXML path name of the current element
            self._node_to_arxml_path[elem] = elem_path

            # if the current element is a package, update the ARXML
            # package path
            if elem.tag == f'{{{self.xml_namespace}}}AR-PACKAGE':
                cur_package_path = f'{cur_package_path}/{short_name}'

            # handle reference bases (for relative references)
            if elem.tag == f'{{{self.xml_namespace}}}REFERENCE-BASE':
                refbase_name = elem.find('./ns:SHORT-LABEL',
                                         self._xml_namespaces).text.strip()
                refbase_path = elem.find('./ns:PACKAGE-REF',
                                         self._xml_namespaces).text.strip()

                is_default = elem.find('./ns:IS-DEFAULT', self._xml_namespaces)

                if is_default is not None:
                    is_default = (is_default.text.strip().lower() == "true")

                current_default_refbase_path = \
                    self._package_default_refbase_path.get(cur_package_path)

                if is_default and current_default_refbase_path is not None:
                    raise ValueError(f'Multiple default reference bases bases '
                                     f'specified for package '
                                     f'"{cur_package_path}".')
                elif is_default:
                    self._package_default_refbase_path[cur_package_path] = \
                        refbase_path

                is_global = elem.find('./ns:IS-GLOBAL', self._xml_namespaces)

                if is_global is not None:
                    is_global = (is_global.text.strip().lower() == "true")

                if is_global:
                    raise ValueError(f'Non-canonical relative references are '
                                     f'not yet supported.')

                # ensure that a dictionary for the refbases of the package exists
                if cur_package_path not in self._package_refbase_paths:
                    self._package_refbase_paths[cur_package_path] = {}
                elif refbase_name in \
                     self._package_refbase_paths[cur_package_path]:
                    raise ValueError(f'Package "{cur_package_path}" specifies '
                                     f'multiple reference bases named '
                                     f'"{refbase_name}".')
                self._package_refbase_paths[cur_package_path][refbase_name] = \
                    refbase_path

            # iterate over all children and add all references contained therein
            for child in elem:
                add_sub_references(child, elem_path, cur_package_path)

        self._arxml_path_to_node = {}
        add_sub_references(self._root, '')

    def _get_arxml_children(self, base_elems, children_location):
        """Locate a set of ElementTree child nodes at a given location.

        This is a method that retrieves a list of ElementTree nodes
        that match a given ARXML location. An ARXML location is a list
        of strings that specify the nesting order of the XML tag
        names; potential references for entries are preceeded by an
        '&': If a sub-element exhibits the specified name, it is used
        directly and if there is a sub-node called
        '{child_tag_name}-REF', it is assumed to contain an ARXML
        reference. This reference is then resolved and the remaining
        location specification is relative to the result of that
        resolution. If a location atom is preceeded by '*', then
        multiple sub-elements are possible. The '&' and '*' qualifiers
        may be combined.

        Example:

        .. code:: text

          # Return all frame triggerings in any physical channel of a
          # CAN cluster, where each conditional, each the physical
          # channel and its individual frame triggerings can be
          # references
          loader._get_arxml_children(can_cluster,
                                     [
                                         'CAN-CLUSTER-VARIANTS',
                                         '*&CAN-CLUSTER-CONDITIONAL',
                                         'PHYSICAL-CHANNELS',
                                         '*&CAN-PHYSICAL-CHANNEL',
                                         'FRAME-TRIGGERINGS',
                                         '*&CAN-FRAME-TRIGGERING'
                                     ])

        """

        if base_elems is None:
            raise ValueError(
                'Cannot retrieve a child element of a non-existing node!')

        # make sure that the children_location is a list. for convenience we
        # also allow it to be a string. In this case we take it that a
        # direct child node needs to be found.
        if isinstance(children_location, str):
            children_location = [ children_location ]

        # make sure that the base elements are iterable. for
        # convenience we also allow it to be an individiual node.
        if type(base_elems).__name__ == 'Element':
            base_elems = [base_elems]

        for child_tag_name in children_location:

            if len(base_elems) == 0:
                return [] # the base elements left are the empty set...

            # handle the set and reference specifiers of the current
            # sub-location
            allow_references = '&' in child_tag_name[:2]
            is_nodeset = '*' in child_tag_name[:2]

            if allow_references:
                child_tag_name = child_tag_name[1:]

            if is_nodeset:
                child_tag_name = child_tag_name[1:]

            # traverse the specified path one level deeper
            result = []

            for base_elem in base_elems:
                local_result = []

                for child_elem in base_elem:
                    ctt = f'{{{self.xml_namespace}}}{child_tag_name}'
                    cttr = f'{{{self.xml_namespace}}}{child_tag_name}-REF'

                    if child_elem.tag == ctt:
                        local_result.append(child_elem)
                    elif child_elem.tag == cttr:
                        tmp = self._follow_arxml_reference(
                            base_elem=base_elem,
                            arxml_path=child_elem.text,
                            dest_tag_name=child_elem.attrib.get('DEST'),
                            refbase_name=child_elem.attrib.get('BASE'))

                        if tmp is None:
                            raise ValueError(f'Encountered dangling reference '
                                             f'{child_tag_name}-REF: '
                                             f'{child_elem.text}')

                        local_result.append(tmp)

                if not is_nodeset and len(local_result) > 1:
                    raise ValueError(f'Encountered a a non-unique child node '
                                     f'of type {child_tag_name} which ought to '
                                     f'be unique')

                result.extend(local_result)

            base_elems = result

        return base_elems

    def _get_unique_arxml_child(self, base_elem, child_location):
        """This method does the same as get_arxml_children, but it assumes
        that the location yields at most a single node.

        It returns None if no match was found and it raises ValueError
        if multiple nodes match the location, i.e., the returned
        object can be used directly if the corresponding node is
        assumed to be present.
        """
        tmp = self._get_arxml_children(base_elem, child_location)

        if len(tmp) == 0:
            return None
        elif len(tmp) == 1:
            return tmp[0]
        else:
            raise ValueError(f'{child_location} does not resolve into a '
                             f'unique node')

    def _get_can_frame(self, can_frame_triggering):
        return self._get_unique_arxml_child(can_frame_triggering, '&FRAME')

    def _get_i_signal(self, i_signal_to_i_pdu_mapping):
        if self.autosar_version_newer(4):
            return self._get_unique_arxml_child(i_signal_to_i_pdu_mapping,
                                                '&I-SIGNAL')
        else:
            return self._get_unique_arxml_child(i_signal_to_i_pdu_mapping,
                                                '&SIGNAL')

    def _get_pdu(self, can_frame):
        return self._get_unique_arxml_child(can_frame,
                                            [
                                                'PDU-TO-FRAME-MAPPINGS',
                                                '&PDU-TO-FRAME-MAPPING',
                                                '&PDU'
                                            ])

    def _get_pdu_path(self, can_frame):
        pdu_ref = self._get_unique_arxml_child(can_frame,
                                               [
                                                   'PDU-TO-FRAME-MAPPINGS',
                                                   '&PDU-TO-FRAME-MAPPING',
                                                   'PDU-REF'
                                               ])
        if pdu_ref is not None:
            pdu_ref = self._get_absolute_arxml_path(pdu_ref,
                                                    pdu_ref.text,
                                                    pdu_ref.attrib.get('BASE'))

        return pdu_ref

    def _get_compu_method(self, system_signal):
        if self.autosar_version_newer(4):
            return self._get_unique_arxml_child(system_signal,
                                                [
                                               '&PHYSICAL-PROPS',
                                               'SW-DATA-DEF-PROPS-VARIANTS',
                                               '&SW-DATA-DEF-PROPS-CONDITIONAL',
                                               '&COMPU-METHOD'
                                                ])
        else:
            return self._get_unique_arxml_child(system_signal,
                                                [
                                                    '&DATA-TYPE',
                                                    'SW-DATA-DEF-PROPS',
                                                    '&COMPU-METHOD'
                                                ])

    def _get_sw_base_type(self, i_signal):
        return self._get_unique_arxml_child(i_signal,
                                            [
                                               '&NETWORK-REPRESENTATION-PROPS',
                                               'SW-DATA-DEF-PROPS-VARIANTS',
                                               '&SW-DATA-DEF-PROPS-CONDITIONAL',
                                               '&BASE-TYPE'
                                            ])

# The ARXML XML namespace for the EcuExtractLoader
NAMESPACE = 'http://autosar.org/schema/r4.0'
NAMESPACES = {'ns': NAMESPACE}

ROOT_TAG = '{{{}}}AUTOSAR'.format(NAMESPACE)

# ARXML XPATHs used by the EcuExtractLoader
def make_xpath(location):
    return './ns:' + '/ns:'.join(location)

ECUC_VALUE_COLLECTION_XPATH = make_xpath([
    'AR-PACKAGES',
    'AR-PACKAGE',
    'ELEMENTS',
    'ECUC-VALUE-COLLECTION'
])
ECUC_MODULE_CONFIGURATION_VALUES_REF_XPATH = make_xpath([
    'ECUC-VALUES',
    'ECUC-MODULE-CONFIGURATION-VALUES-REF-CONDITIONAL',
    'ECUC-MODULE-CONFIGURATION-VALUES-REF'
])
ECUC_REFERENCE_VALUE_XPATH = make_xpath([
    'REFERENCE-VALUES',
    'ECUC-REFERENCE-VALUE'
])
DEFINITION_REF_XPATH = make_xpath(['DEFINITION-REF'])
VALUE_XPATH = make_xpath(['VALUE'])
VALUE_REF_XPATH = make_xpath(['VALUE-REF'])
SHORT_NAME_XPATH = make_xpath(['SHORT-NAME'])
PARAMETER_VALUES_XPATH = make_xpath(['PARAMETER-VALUES'])
REFERENCE_VALUES_XPATH = make_xpath([
    'REFERENCE-VALUES'
])

class EcuExtractLoader(object):

    def __init__(self,
                 root:Any,
                 strict:bool,
                 sort_signals:type_sort_signals=sort_signals_by_start_bit):
        self.root = root
        self.strict = strict
        self.sort_signals = sort_signals

    def load(self) -> InternalDatabase:
        buses:List[Bus] = []
        messages = []
        version = None

        ecuc_value_collection = self.root.find(ECUC_VALUE_COLLECTION_XPATH,
                                               NAMESPACES)
        values_refs = ecuc_value_collection.iterfind(
            ECUC_MODULE_CONFIGURATION_VALUES_REF_XPATH,
            NAMESPACES)
        com_xpaths = [
            value_ref.text
            for value_ref in values_refs
            if value_ref.text.endswith('/Com')
        ]

        if len(com_xpaths) != 1:
            raise ValueError(
                'Expected 1 /Com, but got {}.'.format(len(com_xpaths)))

        com_config = self.find_com_config(com_xpaths[0] + '/ComConfig')

        for ecuc_container_value in com_config:
            definition_ref = ecuc_container_value.find(DEFINITION_REF_XPATH,
                                                       NAMESPACES).text

            if not definition_ref.endswith('ComIPdu'):
                continue

            message = self.load_message(ecuc_container_value)

            if message is not None:
                messages.append(message)

        return InternalDatabase(messages,
                                [],
                                buses,
                                version)

    def load_message(self, com_i_pdu):
        # Default values.
        interval = None
        senders = []
        comments = None

        # Name, frame id, length and is_extended_frame.
        name = com_i_pdu.find(SHORT_NAME_XPATH, NAMESPACES).text
        direction = None

        for parameter, value in self.iter_parameter_values(com_i_pdu):
            if parameter == 'ComIPduDirection':
                direction = value
                break

        com_pdu_id_ref = None

        for reference, value in self.iter_reference_values(com_i_pdu):
            if reference == 'ComPduIdRef':
                com_pdu_id_ref = value
                break

        if com_pdu_id_ref is None:
            raise ValueError('No ComPduIdRef reference found.')

        if direction == 'SEND':
            frame_id, length, is_extended_frame = self.load_message_tx(
                com_pdu_id_ref)
        elif direction == 'RECEIVE':
            frame_id, length, is_extended_frame = self.load_message_rx(
                com_pdu_id_ref)
        else:
            raise NotImplementedError(
                'Direction {} not supported.'.format(direction))

        if frame_id is None:
            LOGGER.warning('No frame id found for message %s.', name)

            return None

        if is_extended_frame is None:
            LOGGER.warning('No frame type found for message %s.', name)

            return None

        if length is None:
            LOGGER.warning('No length found for message %s.', name)

            return None

        # ToDo: interval, senders, comments

        # Find all signals in this message.
        signals = []
        values = com_i_pdu.iterfind(ECUC_REFERENCE_VALUE_XPATH,
                                    NAMESPACES)

        for value in values:
            definition_ref = value.find(DEFINITION_REF_XPATH,
                                        NAMESPACES).text
            if not definition_ref.endswith('ComIPduSignalRef'):
                continue

            value_ref = value.find(VALUE_REF_XPATH, NAMESPACES)
            signal = self.load_signal(value_ref.text)

            if signal is not None:
                signals.append(signal)

        return Message(frame_id=frame_id,
                       is_extended_frame=is_extended_frame,
                       name=name,
                       length=length,
                       senders=senders,
                       send_type=None,
                       cycle_time=interval,
                       signals=signals,
                       comment=comments,
                       bus_name=None,
                       strict=self.strict,
                       sort_signals=self.sort_signals)

    def load_message_tx(self, com_pdu_id_ref):
        return self.load_message_rx_tx(com_pdu_id_ref,
                                       'CanIfTxPduCanId',
                                       'CanIfTxPduDlc',
                                       'CanIfTxPduCanIdType')

    def load_message_rx(self, com_pdu_id_ref):
        return self.load_message_rx_tx(com_pdu_id_ref,
                                       'CanIfRxPduCanId',
                                       'CanIfRxPduDlc',
                                       'CanIfRxPduCanIdType')

    def load_message_rx_tx(self,
                           com_pdu_id_ref,
                           parameter_can_id,
                           parameter_dlc,
                           parameter_can_id_type):
        can_if_tx_pdu_cfg = self.find_can_if_rx_tx_pdu_cfg(com_pdu_id_ref)
        frame_id = None
        length = None
        is_extended_frame = None

        if can_if_tx_pdu_cfg is not None:
            for parameter, value in self.iter_parameter_values(can_if_tx_pdu_cfg):
                if parameter == parameter_can_id:
                    frame_id = int(value)
                elif parameter == parameter_dlc:
                    length = int(value)
                elif parameter == parameter_can_id_type:
                    is_extended_frame = (value == 'EXTENDED_CAN')

        return frame_id, length, is_extended_frame

    def load_signal(self, xpath):
        ecuc_container_value = self.find_value(xpath)
        if ecuc_container_value is None:
            return None

        name = ecuc_container_value.find(SHORT_NAME_XPATH, NAMESPACES).text

        # Default values.
        is_signed = False
        is_float = False
        minimum = None
        maximum = None
        factor = 1
        offset = 0
        unit = None
        choices = None
        comments = None
        receivers = []
        decimal = SignalDecimal(Decimal(factor), Decimal(offset))

        # Bit position, length, byte order, is_signed and is_float.
        bit_position = None
        length = None
        byte_order = None

        for parameter, value in self.iter_parameter_values(ecuc_container_value):
            if parameter == 'ComBitPosition':
                bit_position = int(value)
            elif parameter == 'ComBitSize':
                length = int(value)
            elif parameter == 'ComSignalEndianness':
                byte_order = value.lower()
            elif parameter == 'ComSignalType':
                if value in ['SINT8', 'SINT16', 'SINT32']:
                    is_signed = True
                elif value in ['FLOAT32', 'FLOAT64']:
                    is_float = True

        if bit_position is None:
            LOGGER.warning('No bit position found for signal %s.',name)

            return None

        if length is None:
            LOGGER.warning('No bit size found for signal %s.', name)

            return None

        if byte_order is None:
            LOGGER.warning('No endianness found for signal %s.', name)

            return None

        # ToDo: minimum, maximum, factor, offset, unit, choices,
        #       comments and receivers.

        return Signal(name=name,
                      start=bit_position,
                      length=length,
                      receivers=receivers,
                      byte_order=byte_order,
                      is_signed=is_signed,
                      scale=factor,
                      offset=offset,
                      minimum=minimum,
                      maximum=maximum,
                      unit=unit,
                      choices=choices,
                      comment=comments,
                      is_float=is_float,
                      decimal=decimal)

    def find_com_config(self, xpath):
        return self.root.find(make_xpath([
            "AR-PACKAGES",
            "AR-PACKAGE/[ns:SHORT-NAME='{}']".format(xpath.split('/')[1]),
            "ELEMENTS",
            "ECUC-MODULE-CONFIGURATION-VALUES/[ns:SHORT-NAME='Com']",
            "CONTAINERS",
            "ECUC-CONTAINER-VALUE/[ns:SHORT-NAME='ComConfig']",
            "SUB-CONTAINERS"
        ]),
                              NAMESPACES)

    def find_value(self, xpath):
        return self.root.find(make_xpath([
            "AR-PACKAGES",
            "AR-PACKAGE/[ns:SHORT-NAME='{}']".format(xpath.split('/')[1]),
            "ELEMENTS",
            "ECUC-MODULE-CONFIGURATION-VALUES/[ns:SHORT-NAME='Com']",
            "CONTAINERS",
            "ECUC-CONTAINER-VALUE/[ns:SHORT-NAME='ComConfig']",
            "SUB-CONTAINERS",
            "ECUC-CONTAINER-VALUE/[ns:SHORT-NAME='{}']".format(xpath.split('/')[-1])
        ]),
                              NAMESPACES)

    def find_can_if_rx_tx_pdu_cfg(self, com_pdu_id_ref):
        messages = self.root.iterfind(
            make_xpath([
                "AR-PACKAGES",
                "AR-PACKAGE/[ns:SHORT-NAME='{}']".format(
                    com_pdu_id_ref.split('/')[1]),
                "ELEMENTS",
                "ECUC-MODULE-CONFIGURATION-VALUES/[ns:SHORT-NAME='CanIf']",
                'CONTAINERS',
                "ECUC-CONTAINER-VALUE/[ns:SHORT-NAME='CanIfInitCfg']",
                'SUB-CONTAINERS',
                'ECUC-CONTAINER-VALUE'
            ]),
            NAMESPACES)

        for message in messages:
            definition_ref = message.find(DEFINITION_REF_XPATH,
                                          NAMESPACES).text

            if definition_ref.endswith('CanIfTxPduCfg'):
                expected_reference = 'CanIfTxPduRef'
            elif definition_ref.endswith('CanIfRxPduCfg'):
                expected_reference = 'CanIfRxPduRef'
            else:
                continue

            for reference, value in self.iter_reference_values(message):
                if reference == expected_reference:
                    if value == com_pdu_id_ref:
                        return message

    def iter_parameter_values(self, param_conf_container):
        parameters = param_conf_container.find(PARAMETER_VALUES_XPATH,
                                               NAMESPACES)

        if parameters is None:
            raise ValueError('PARAMETER-VALUES does not exist.')

        for parameter in parameters:
            definition_ref = parameter.find(DEFINITION_REF_XPATH,
                                            NAMESPACES).text
            value = parameter.find(VALUE_XPATH, NAMESPACES).text
            name = definition_ref.split('/')[-1]

            yield name, value

    def iter_reference_values(self, param_conf_container):
        references = param_conf_container.find(REFERENCE_VALUES_XPATH,
                                               NAMESPACES)

        if references is None:
            raise ValueError('REFERENCE-VALUES does not exist.')

        for reference in references:
            definition_ref = reference.find(DEFINITION_REF_XPATH,
                                            NAMESPACES).text
            value = reference.find(VALUE_REF_XPATH, NAMESPACES).text
            name = definition_ref.split('/')[-1]

            yield name, value

def is_ecu_extract(root):
    ecuc_value_collection = root.find(ECUC_VALUE_COLLECTION_XPATH,
                                      NAMESPACES)

    return ecuc_value_collection is not None

def load_string(string:str,
                strict:bool=True,
                sort_signals:type_sort_signals=sort_signals_by_start_bit) \
            -> InternalDatabase:
    """Parse given ARXML format string.

    """

    root = ElementTree.fromstring(string)

    m = re.match(r'{(.*)}AUTOSAR', root.tag)
    if not m:
        raise ValueError(f"No XML namespace specified or illegal root tag name '{root.tag}'")
    xml_namespace = m.group(1)

    # Should be replaced with a validation using the XSD file.
    recognized_namespace = False
    if re.match(r'http://autosar.org/schema/r(4.*)', xml_namespace) \
       or re.match(r'http://autosar.org/(3.*)', xml_namespace) \
       or re.match(r'http://autosar.org/(.*)\.DAI\.[0-9]', xml_namespace):
        recognized_namespace = True

    if not recognized_namespace:
        raise ValueError(f"Unrecognized XML namespace '{xml_namespace}'")

    if is_ecu_extract(root):
        if root.tag != ROOT_TAG:
            raise ValueError(
                'Expected root element tag {}, but got {}.'.format(
                    ROOT_TAG,
                    root.tag))

        return EcuExtractLoader(root, strict, sort_signals).load()
    else:
        return SystemLoader(root, strict, sort_signals).load()
