#pragma once

/**
 * Display. Real implementation on CM4 (depends on SDRAM; call after sdram_init()).
 * No-op stubs on CM7.
 */
void display_init(void);

/**
 * Draw four coloured squares at the screen corners to verify display coordinates.
 * (0,0) top-left = red, top-right = green, bottom-left = blue, bottom-right = white.
 * Fills the rest of the screen black. Use to confirm which physical corner is which.
 */
void display_draw_corner_markers(void);

/**
 * Draw two unfilled 1-pixel boxes: outer on the visible area (0,0)-(478,270),
 * inner one pixel inset. Use to verify drawing aligns to screen edges.
 * (Panel often does not show last column/row of 480x272.)
 * Fills the rest of the screen black.
 */
void display_draw_edge_boxes(void);
