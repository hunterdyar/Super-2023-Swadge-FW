/*
 * bresenham.h
 *
 *  Created on: Mar 3, 2019
 *      Author: Adam Feinstein
 */

#ifndef SRC_BRESENHAM_H_
#define SRC_BRESENHAM_H_

#include "display.h"

void oddEvenFill(display_t* disp, int x0, int y0, int x1, int y1,
                 paletteColor_t boundaryColor, paletteColor_t fillColor);

void plotLine(display_t* disp, int x0, int y0, int x1, int y1, paletteColor_t col, int dashWidth);
void plotRect(display_t*, int x0, int y0, int x1, int y1, paletteColor_t col);
void plotEllipse(display_t*, int xm, int ym, int a, int b, paletteColor_t col);
void plotOptimizedEllipse(display_t*, int xm, int ym, int a, int b, paletteColor_t col);
void plotCircle(display_t*, int xm, int ym, int r, paletteColor_t col);
void plotCircleQuadrants(display_t* disp, int xm, int ym, int r, bool q1,
                         bool q2, bool q3, bool q4, paletteColor_t col);
void plotCircleFilled(display_t* disp, int xm, int ym, int r, paletteColor_t col);
void plotEllipseRect(display_t*, int x0, int y0, int x1, int y1, paletteColor_t col);
void plotQuadBezierSeg(display_t*, int x0, int y0, int x1, int y1, int x2, int y2, paletteColor_t col);
void plotQuadBezier(display_t*, int x0, int y0, int x1, int y1, int x2, int y2, paletteColor_t col);
void plotQuadRationalBezierSeg(display_t*, int x0, int y0, int x1, int y1, int x2, int y2, float w, paletteColor_t col);
void plotQuadRationalBezier(display_t*, int x0, int y0, int x1, int y1, int x2, int y2, float w, paletteColor_t col);
void plotRotatedEllipse(display_t*, int x, int y, int a, int b, float angle, paletteColor_t col);
void plotRotatedEllipseRect(display_t*, int x0, int y0, int x1, int y1, long zd, paletteColor_t col);
void plotCubicBezierSeg(display_t*, int x0, int y0, float x1, float y1, float x2, float y2, int x3, int y3,
                        paletteColor_t col);
void plotCubicBezier(display_t*, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, paletteColor_t col);
void plotQuadSpline(display_t*, int n, int x[], int y[], paletteColor_t col);
void plotCubicSpline(display_t*, int n, int x[], int y[], paletteColor_t col);

#endif /* SRC_BRESENHAM_H_ */
