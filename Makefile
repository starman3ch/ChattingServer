CC = g++
CFAGS = -W -Wall
TARGET = ChatServer
OBJS = ServerSession.o ChatServer.o ServerDB.o ServerMain.o

# all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFAGS) $^ -o $@ -lboost_system


clean :
	rm *.o *.gch $(TARGET)
