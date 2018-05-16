Motor tester
============

#. Setup a virtual CAN interface on your Linux machine.

   .. code-block::
   
      > sudo modprobe vcan
      > sudo ip link add dev vcan0 type vcan
      > sudo ip link set up vcan0
      >

#. Start the motor simulator in terminal 1.

   .. code-block::
   
      > python3 motor.py

#. Run the test script in terminal 2.

   .. code-block::
   
      > python3 main.py
      Motor speed is 0 rpm and load is 0%.
      Setting motor speed to 50 rpm.
      Motor speed is 50 rpm and load is 12%.
      Setting motor speed to 0 rpm.
      Motor speed is 0 rpm and load is 12%.
      Setting motor speed to 65535 rpm to terminate it.
      Done!
      >

#. See the motor simulator output in terminal 1.

   .. code-block::
   
      Received motor speed of 50 rpm.
      Received motor speed of 0 rpm.
      Received motor speed of 65535 rpm.
      Done!
      >
