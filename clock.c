#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <string.h>

// TI$ variable location (absolute address)
#define TI$ 0x0277

// PETSCII character mappings for digits 0-9
char digits[10][5] = {
    {0x30, 0x30, 0x30, 0x30, 0x30}, // 0
    {0x31, 0x31, 0x31, 0x31, 0x31}, // 1
    {0x32, 0x32, 0x32, 0x32, 0x32}, // 2
    {0x33, 0x33, 0x33, 0x33, 0x33}, // 3
    {0x34, 0x34, 0x34, 0x34, 0x34}, // 4
    {0x35, 0x35, 0x35, 0x35, 0x35}, // 5
    {0x36, 0x36, 0x36, 0x36, 0x36}, // 6
    {0x37, 0x37, 0x37, 0x37, 0x37}, // 7
    {0x38, 0x38, 0x38, 0x38, 0x38}, // 8
    {0x39, 0x39, 0x39, 0x39, 0x39}  // 9
};

// Function to display a digit at specified position
void display_digit(int digit, int x, int y) {
    // Set cursor position
    gotoxy(x, y);
    
    // Display the digit
    cprintf("%c", digits[digit][0]);
}

// Function to display time in PETSCII format
void display_time() {
    // Get current time
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    // Format time as HH:MM:SS
    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
    
    // Display time
    gotoxy(10, 10);
    cprintf("%s", buffer);
}

int main() {
    // Clear screen
    clrscr();
    
    // Display digital clock
    display_time();
    
    // Wait for keypress
    cgetc();
    
    return 0;
}
