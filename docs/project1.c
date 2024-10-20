#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define SLOT_DURATION 10  // microseconds
#define SIFS_DURATION 2   // slots
#define DIFS_DURATION 4   // slots
#define CW0 8  // Minimum contention window size
#define CW_MAX 1024
#define FRAME_SIZE 1500  // bytes
#define SIMULATION_TIME 10  // seconds
#define NUM_STATIONS 2
#define NUM_LAMBDA_VALUES 6  // Number of different λ values (100, 200, 300, ... 1000)

// Structure for representing each station
typedef struct {
    int backoff;  // Backoff counter
    int collisions;  // Number of collisions experienced
    double throughput;  // Total throughput
    int frames_transmitted;  // Total frames transmitted
} Station;

// Function to generate a random number in range [min, max]
int get_random(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

// Function to simulate CSMA/CA for a station
void simulate_csma(Station *station, int lambda, int topology) {
    int CW = CW0;
    int time = 0;  // Time in microseconds
    
    while (time < SIMULATION_TIME * 1000000) {  // Convert seconds to microseconds
        // Randomly generate the arrival of frames based on Poisson process
        if (rand() % 1000 < lambda) {
            // New frame arrives, calculate backoff
            station->backoff = get_random(0, CW);
            
            // Start backoff countdown
            while (station->backoff > 0) {
                time += SLOT_DURATION;
                station->backoff--;
                
                // Simulate collision or transmission depending on topology
                if (topology == 1) {
                    // Single Collision Domain: All stations hear each other
                    if (rand() % 100 < 5) {  // Assuming 5% chance of collision
                        station->collisions++;
                        CW = fmin(CW * 2, CW_MAX);  // Double contention window
                        break;
                    }
                } else if (topology == 2) {
                    // Hidden Terminal: Stations A and B don't hear each other
                    if (rand() % 100 < 10) {  // Higher chance of collision in hidden terminal case
                        station->collisions++;
                        CW = fmin(CW * 2, CW_MAX);
                        break;
                    }
                }
            }
            
            if (station->backoff == 0) {
                // Successful transmission
                station->frames_transmitted++;
                station->throughput += FRAME_SIZE;
                CW = CW0;  // Reset contention window on success
            }
        }
        time += SLOT_DURATION;
    }
}

// Main function
int main() {
    srand(time(NULL));  // Initialize random seed

    Station stations[NUM_STATIONS];
    int lambda_values[NUM_LAMBDA_VALUES] = {100, 200, 300, 500, 700, 1000};  // Arrival rates
    int topology = 1;  // 1 = Single Collision Domain, 2 = Hidden Terminals

    // Run simulations for each lambda value
    for (int l = 0; l < NUM_LAMBDA_VALUES; l++) {
        int lambda = lambda_values[l];
        printf("Simulating for λ = %d frames/sec\n", lambda);

        // Reset station stats for each simulation
        for (int i = 0; i < NUM_STATIONS; i++) {
            stations[i].backoff = 0;
            stations[i].collisions = 0;
            stations[i].throughput = 0;
            stations[i].frames_transmitted = 0;
        }
        
        // Simulate CSMA/CA for each station
        for (int i = 0; i < NUM_STATIONS; i++) {
            simulate_csma(&stations[i], lambda, topology);
        }
        
        // Calculate and display results for each station
        double fairness_index = (double)stations[0].frames_transmitted / stations[1].frames_transmitted;
        for (int i = 0; i < NUM_STATIONS; i++) {
            printf("Station %d - Throughput: %.2f KBps, Collisions: %d, Frames Transmitted: %d\n",
                   i + 1,
                   stations[i].throughput / 1000,  // Convert bytes to KB
                   stations[i].collisions,
                   stations[i].frames_transmitted);
        }
        printf("Fairness Index: %.2f\n", fairness_index);
        printf("---------------------------------\n");
    }
    
    return 0;
}
