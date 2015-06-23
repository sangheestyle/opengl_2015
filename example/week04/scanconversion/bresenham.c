/*
 *  Scan Convert Line (Bresenham Algorithm)
 *  Draw line from (x0,y0) to (x1,y1) in pixel coordinates
 */

void scanline(int x0,int y0,int x1,int y1)
{
   // Draw starting pixel
   DrawPixel(x0,y0);
   // Start!=End
   if (x0!=x1 || y0!=y1)
   {
      //  Distance in X and Y directions
      int dx = abs(x1-x0);
      int dy = abs(y1-y0);
      //  Increment to move from P0 to P1
      int ix = (x1<x0) ? -1 : +1;
      int iy = (y1<y0) ? -1 : +1;

      // Nearer to X axis
      if (dx>=dy)
      {
         int x;                //  Counter
         int y = y0;           //  Initial y value
         int F = 2*dy-dx;      //  Decision variable
         int dF0 = 2*dy;       //  Decision increment same y
         int dF1 = 2*(dy-dx);  //  Decision increment next y
         //  Loop over x values
         for (x=x0+ix ; x!=x1 ; x+=ix)
         {
            //  Increment y
            if (F>0)
            {
               y += iy;
               F += dF1;
            }
            //  Keep y
            else
               F += dF0;
            //  Draw pixel
            DrawPixel(x,y);
         }
      }
      // Nearer to Y axis
      else
      {
         int y;                //  Counter
         int x = x0;           //  initial x value
         int F = 2*dx-dy;      //  Decision variable
         int dF0 = 2*dx;       //  Decision increment same x
         int dF1 = 2*(dx-dy);  //  Decision increment next x
         //  Loop over y values
         for (y=y0+iy ; y!=y1 ; y+=iy)
         {
            //  Increment x
            if (F>0)
            {
               x += ix;
               F += dF1;
            }
            //  Keep x
            else
               F += dF0;
            //  Draw pixel
            DrawPixel(x,y);
         }
      }

      //  Draw last pixel
      DrawPixel(x1,y1);
   }
}
