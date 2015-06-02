/* C Example */
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include "mpi_hello.h"
#define MSG_SIZE 2

int rank;
int lockerRooms[3] = {0,0,0};
int myLockerId= -1;

int msg[MSG_SIZE];
int size;
MPI_Status status;

//sprawdzamy bufor wiadomosci przychodzacych
void checkBuffor(){


}

void enter_to_locker(){
	printf("%d: Weszłem do budynku\n",rank);
	msg[0] = TRY_ENTER_MSG;
	//MPI_Bcast(msg, MSG_SIZE, MPI_INT, rank, MPI_COMM_WORLD);
	int i;
	MPI_Comm_size( MPI_COMM_WORLD, &size );
	printf("%d: Na basenie jest %d ludzi\n",rank, size);
	for (i = 0; i < size; ++i){
		if (i != rank) {
			msg[1] = i;
			MPI_Send(msg, MSG_SIZE, MPI_INT, i, TRY_ENTER_MSG, MPI_COMM_WORLD );
			printf("%d: Wysłałem do: %d\n",rank, msg[1]);
		}
	}
	
	for (i = 1; i < size; ++i){
		printf("%d: Czekam na odpowiedz od %d osob\n",rank, size -1);
		i += msg_receive();
	}
	
	for (i = 0; i < size; ++i){
		if (lockerRooms[i] < LOCKER_SIZE){
			if (myLockerId == -1) {
				myLockerId = i;
			}else{
				//jesli sa dwie szatnie zapełnione osobami naszej plci				
				//wybieramy szatnie w ktorej jest większe zapelnienie
				if (lockerRooms[myLockerId] < lockerRooms[i]){
					myLockerId = i;
				}
			}		
		}
	}

	printf("%d: Wchodze do szatni nr %d\n", rank, myLockerId);

	++lockerRooms[myLockerId];

	printf("\nSzatnia 0 = %d\n", lockerRooms[0]);
	printf("Szatnia 1 = %d\n", lockerRooms[1]);
	printf("Szatnia 2 = %d\n\n", lockerRooms[2]);

}

int msg_receive(){
	MPI_Recv( msg, MSG_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	if(msg[0] == TRY_ENTER_MSG){
		int receiver = msg[1];
		msg[0] = LOCKER_ID_MSG;
		msg[1] = myLockerId;
		printf("%d: Dostalem prosbe o podanie szatni, odp = %d\n", rank, msg[1]);
		MPI_Send( msg, MSG_SIZE, MPI_INT, receiver, LOCKER_ID_MSG, MPI_COMM_WORLD );
		return -1;
	}

	if(msg[0] == LOCKER_ID_MSG){
		int customerLockerId = msg[1];
		if (customerLockerId > -1) {
			lockerRooms[customerLockerId] += 1;
			printf("%d: Dodałem do szatni %d\n", rank, msg[1]);
		}
		return 0;
	}

}

void return_to_locker(){
	sleep(1);
	myLockerId = -1;
	printf("%d: Do widzenia\n\n", rank);
}

int main (int argc, char* argv[])
{
  	int sender;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	enter_to_locker();

	//swiming 5s
	sleep(5);

	return_to_locker();

	MPI_Finalize();
	printf("%d: Ide spac\n", rank);
	return 0;
}
