#include <xparameters.h>
#include "xil_io.h"
#include "sevenSeg_new.h"

// Add segment definitions for letters A-F
static const int digit_segs[16] = {
    0b0111111, // 0
    0b0000110, // 1
    0b1011011, // 2
    0b1001111, // 3
    0b1100110, // 4
    0b1101101, // 5
    0b1111101, // 6
    0b0000111, // 7
    0b1111111, // 8
    0b1101111, // 9
    0b1110111, // A
    0b1111100, // b
    0b0111001, // C
    0b1011110, // d
    0b1111001, // E
    0b1110001, // F
};

static int current_display_state = 0;
char output1; // Variable to store the first displayed character
char output2; // Variable to store the second displayed character

void sevenseg_draw_digit(int position, int value) {
    if (value < 0 || value > 15) {
        xil_printf("Error: Invalid value for seven-segment display: %d\r\n", value);
        return; // Ensure the value is within the valid range
    }

    int segs, segs_mask, digit_mask;

    // Get the segment encoding for the value
    segs = digit_segs[value];
    segs_mask = 127 ^ segs; // Invert to set the active segments
    digit_mask = 255 ^ (1 << position); // Set the position mask

    // Update the current display state by combining with the new digit
    current_display_state &= ~(127 | (255 << 7)); // Clear existing digit and segment
    current_display_state |= (segs_mask | (digit_mask << 7)); // Combine with the new digit

    // Store the character representation of the displayed value in output1 or output2
    if (position == 0) {
        output1 = (value < 10) ? ('0' + value) : ('A' + value - 10); // Map value to '0'-'9' or 'A'-'F'
    } else if (position == 1) {
        output2 = (value < 10) ? ('0' + value) : ('A' + value - 10); // Map value to '0'-'9' or 'A'-'F'
    }

    // Debug: Print the updated outputs

    // Write the updated display state to the hardware
    Xil_Out32(XPAR_SEVENSEG_0_S00_AXI_BASEADDR, current_display_state);
}

