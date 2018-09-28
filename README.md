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
	1)  i) Use go language to write the server code. Go has built in Goroutines and Channels supported to concurrency.
	2) i) We choose the color which has a higher rgb values to overwrite the conflict.
 	     ii) We use vector time to determine which user first draws the pixel to determine which one we will use.


-timeline
	1)  week 5~7 client
	2)  week 7~10 server code and socket code
	4)  week 11 features and utilities implementation
	5)  week 12~13 frontend and backend connection
