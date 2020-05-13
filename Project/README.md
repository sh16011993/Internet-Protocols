# ece573-proj

## useful links

POX wiki:  
https://openflow.stanford.edu/display/ONL/POX+Wiki.html

POX examples:  
http://pld.cs.luc.edu/courses/netmgmt/sum17/notes/mininet_and_pox.html

How to send Openflow messages in POX:  
https://github.com/mininet/openflow-tutorial/wiki/Create-a-Learning-Switch#Sending_OpenFlow_messages_with_POX

POX on github:  
https://github.com/att/pox  
useful files to help:  
https://github.com/att/pox/blob/master/pox/forwarding/l2_multi.py  
https://github.com/att/pox/blob/master/pox/openflow/spanning_tree.py  
https://github.com/att/pox/blob/master/pox/misc/of_tutorial.py

This project demonstrates Load Balancing in Software Defined Networks [For Datacenter Networks]
-> proj-topo file defines the topology for the Network (Leaf-Spine Topology)
-> proj-pox file defines the controller logic that dictates the path that the packets will take from the source to the destination. Controller installs flows in the Open vSwitches that guides the path of the packets
