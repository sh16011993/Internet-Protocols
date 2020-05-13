This is the README for the TCP Analysis Project.

ns3-dev is required for this project.

To clone ns3-dev, run the following commands:
cd /home/<user>
git clone https://gitlab.com/nsnam/ns-3-dev.git
cd /home/<user>/ns-3-dev
./waf configure --enable-examples

To compile the tcp_sshekha4.cc file, issue the following commands in the commandline:
cd /home/<user>/ns-3-dev
./waf

The above command will build various modules

To run tcp_sshekha4.cc, issue the following command in the commandline:
./waf --run tcp_sshekha4

Dependency Requirements: None

Note: 
1) For all the 5 experiments, links in the network have a transfer rate of 1 Gbps and 500 MB of data is transferred from each sender to the receiver. 
2) Delay in the links is set to zero. 
3) In all the experiments, senders start sending the data after 1 second of the start of the simulation and stop sending after 45 seconds.
4) Simulation is run for 1 minute to allow for buffer time in case there is a delay in sending the data due to packet losses and hence retransmissions.