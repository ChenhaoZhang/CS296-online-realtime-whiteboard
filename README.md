# CS296-online-realtime-whiteboard
CS296 Project

-features:
        1) draw basic dots and lines with common colors support (transparency support as well)
        2) basic geometry shapes support (circle, square, triangle)
        3) auto fill a closed figure with chosen color and transparency
        4) duplicate a dot, line, and a closed figure.
        5) undo and redo



-technical challenges:
        1) How to keep the concurrency of all the users and obtain a relative low latency.
        2) A reasonable way to deal with the potential conflicts between users.
        3) How to draw basic lines and dots using mouse

-possible solutions:
        1) i) Use go language to write the server code. Go has built in Goroutines and Channels supported to concurrency. (pros\
: it seems easier to build a distributed system using go,  cons: need to find a way to connect it with c socket)
           ii) Use the optimitic replication(pros: significantly lower the latency, cons: need to figure out how to solve the c\
onfict [which is solved by using vector clock])
        2) i) We choose the color which has a higher rgb values to overwrite the conflict.  (pros: easy to operate, cons: maybe\
 the most recent updated client change the higher rgb values to a lower one.)
           ii) We use vector time to determine which user first draws the pixel to determine which one we will use.(pros: will \
keep everything in order, better than the timestamp since this algorithm do consider the latency of networking, cons: each proc\
ess has to have a huge vector to keep the clock value for every pixel, and everytime a process recieve a message it will take l\
ots of time to update all of the value)
        3)still working on how to use ncurses library


-timeline
        1)  week 5~7 client (window using ncurses)
        2)  week 7~10 server code and socket code
        4)  week 11 features and utilities implementation
        5)  week 12~13 frontend and backend connection

-links
        Go:
		https://notes.shichao.io/gopl/ch8/
        
	Socket:
		https://www.geeksforgeeks.org/socket-programming-cc/
        
	Vector_Clock:
		https://mittaltutorials.wordpress.com/2016/01/03/exp2-vector-clock/
	
	P2P:
		https://www.rti.com/blog/2014/08/01/create-a-p2p-distributed-application-in-under-35-lines-of-c11-code/
		http://cs.berry.edu/~nhamid/p2p/