from typing import Optional, Union


class EnvironmentVariable:
    """A CAN environment variable.

    """

    def __init__(self,
                 name: str,
                 env_type: int,
                 minimum: Union[int, float],
                 maximum: Union[int, float],
                 unit: str,
                 initial_value: Union[int, float],
                 env_id: int,
                 access_type: str,
                 access_node: str,
                 comment: Optional[str]) -> None:
        self._name = name
        self._env_type = env_type
        self._minimum = minimum
        self._maximum = maximum
        self._unit = unit
        self._initial_value = initial_value
        self._env_id = env_id
        self._access_type = access_type
        self._access_node = access_node
        self._comment = comment

    @property
    def name(self) -> str:
        """The environment variable name as a string.

        """

        return self._name

    @property
    def env_type(self) -> int:
        """The environment variable type value.

        """

        return self._env_type

    @env_type.setter
    def env_type(self, value: int) -> None:
        self._env_type = value

    @property
    def minimum(self) -> Union[int, float]:
        """The minimum value of the environment variable.

        """

        return self._minimum

    @minimum.setter
    def minimum(self, value: Union[int, float]) -> None:
        self._minimum = value

    @property
    def maximum(self) -> Union[int, float]:
        """The maximum value of the environment variable.

        """

        return self._maximum

    @maximum.setter
    def maximum(self, value: Union[int, float]) -> None:
        self._maximum = value

    @property
    def unit(self) -> str:
        """ The units in which the environment variable is expressed as a string.

        """

        return self._unit

    @unit.setter
    def unit(self, value: str) -> None:
        self._unit = value

    @property
    def initial_value(self) -> Union[int, float]:
        """The initial value of the environment variable.

        """

        return self._initial_value

    @initial_value.setter
    def initial_value(self, value: Union[int, float]) -> None:
        self._initial_value = value

    @property
    def env_id(self) -> int:
        """The id value of the environment variable.

        """

        return self._env_id

    @env_id.setter
    def env_id(self, value: int) -> None:
        self._env_id = value

    @property
    def access_type(self) -> str:
        """The environment variable access type as a string.

        """

        return self._access_type

    @access_type.setter
    def access_type(self, value: str) -> None:
        self._access_type = value

    @property
    def access_node(self) -> str:
        """The environment variable access node as a string.

        """

        return self._access_node

    @access_node.setter
    def access_node(self, value: str) -> None:
        self._access_node = value

    @property
    def comment(self) -> Optional[str]:
        """The environment variable comment, or ``None`` if unavailable.

        """

        return self._comment

    @comment.setter
    def comment(self, value: str) -> None:
        self._comment = value

    def __repr__(self) -> str:
        return "environment_variable('{}', {}, {}, {}, '{}', {}, {}, '{}', '{}', {})".format(
            self._name,
            self._env_type,
            self._minimum,
            self._maximum,
            self._unit,
            self._initial_value,
            self._env_id,
            self._access_type,
            self._access_node,
            "'" + self._comment + "'" if self._comment is not None else None)
