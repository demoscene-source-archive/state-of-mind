/************************************************************************
 * A simple DCT algorithm based on:
 * W. H. Chen, C. H. Smith and S. C. Fralick "A fast computational
 * algorithm for the discrete cosine transform," IEEE Trans. Commun.,
 * vol. COM-25, pp. 1004-1009, Sept 1977.
 *
 * Enhanced by Skal/1998
 ************************************************************************/

#define _USE_PRIVATE_

#include "main.h"
#include "./jpeg.h"

#ifdef UNIX

#define fc1d4 (0.70703125/2.0)
#define fc1d4p (0.70703125)

#define fc1d8 (0.923828125/2.0)
#define fc3d8 (0.3828125/2.0)

#define fc1d16 (0.98046875/2.0)
#define fc3d16 (0.83203125/2.0)
#define fc5d16 (0.5546875/2.0)
#define fc7d16 (0.1953125/2.0)

#define	FXCHG(a,b) { FLT Tmp=(a); (a)+=(b); (b)-=Tmp; }
#define	FXCHG_REV(a,b) { FLT Tmp=(a); (a)+=(b); (b)=Tmp-(b); }

EXTERN void Do_Chen_IDCT( FLT In[64] )
{
   INT i;
   FLT Tmp[64], *PTmp, *POut;

   PTmp = Tmp;
   POut = In;
   for( i=8; i>0; --i )
   {
      FLT a1,a2, c0,c1,c3;

      a1 = fc1d4*( In[0]+In[32] );
      a2 = fc1d8*In[16] + fc3d8*In[48];
      FXCHG_REV(a1,a2);
      c3 = fc1d16*In[ 8] + fc7d16*In[56];
      c0 = fc3d16*In[24] + fc5d16*In[40];
      FXCHG(c0,c3);
      PTmp[ 0] = a1+c0;
      PTmp[56] = a1-c0;
      c1 = fc3d16*In[40] - fc5d16*In[24];
      c0 = fc7d16*In[ 8] - fc1d16*In[56];
      FXCHG(c0,c1);
      FXCHG(c1,c3); c1 = fc1d4p*c1; c3 = fc1d4p*c3;

      PTmp[24] = a2+c0;
      PTmp[32] = a2-c0;

      a1 = fc1d4*( In[0]-In[32] );
      a2 = fc3d8*In[16] - fc1d8*In[48];
      FXCHG_REV(a1,a2);
      PTmp[ 8] = a1+c3;
      PTmp[48] = a1-c3;
      PTmp[16] = a2+c1;
      PTmp[40] = a2-c1;

      PTmp++; In++;
   }

   PTmp = Tmp;
   for( i=8; i>0; --i, PTmp+=8, POut+=8 )
   {
      FLT a1,a2,a3, c0,c1,c2,c3;

      c0 = fc7d16*PTmp[1]-fc1d16*PTmp[7];
      c1 = fc3d16*PTmp[5]-fc5d16*PTmp[3];
      FXCHG(c0,c1);
      c2 = fc3d16*PTmp[3]+fc5d16*PTmp[5];
      c3 = fc1d16*PTmp[1]+fc7d16*PTmp[7];
      FXCHG(c2,c3);
      FXCHG(c1,c3); c1 = fc1d4p*c1; c3 = fc1d4p*c3;

      a1 = fc1d4*( PTmp[0]-PTmp[4] );
      a2 = fc1d4*( PTmp[0]+PTmp[4] );
      a3 = fc1d8*PTmp[2] + fc3d8*PTmp[6];
      FXCHG_REV(a2,a3);
      POut[0] = a2+c2;
      POut[7] = a2-c2;
      POut[3] = a3+c0;
      POut[4] = a3-c0;

      a2 = fc3d8*PTmp[2] - fc1d8*PTmp[6];
      FXCHG_REV(a1,a2);
      POut[2] = a2+c1;
      POut[5] = a2-c1;
      POut[6] = a1-c3;
      POut[1] = a1+c3;
   }
}

EXTERN void Do_Chen_DCT( FLT In[64] )
{
   INT i;
   FLT Tmp[64], *PTmp, *POut;

   PTmp = Tmp;
   POut = In;
   for( i=8; i>0; --i, In++, PTmp++ )  // columns
   {
      FLT a0,a1,a2,a3;

      a0 = In[0] + In[56];
      a3 = In[24] + In[32];
      FXCHG_REV(a0,a3);
      a1 = In[8] + In[48];
      a2 = In[16] + In[40];
      FXCHG_REV(a1,a2);
      FXCHG_REV(a0,a1);
      PTmp[0]  = fc1d4*a0;
      PTmp[32] = fc1d4*a1;
      PTmp[16] = fc3d8*a2 + fc1d8*a3;
      PTmp[48] = fc3d8*a3 - fc1d8*a2;

      a2 = In[8] - In[48];
      a1 = In[16] - In[40];
      FXCHG_REV(a2,a1); a1 *= fc1d4p; a2 *= fc1d4p;
      a0 = In[24] - In[32];
      FXCHG_REV(a0,a1);
      a3 = In[0] - In[56];
      FXCHG_REV(a3,a2);

      PTmp[8]  = fc7d16*a0 + fc1d16*a3;
      PTmp[56] = fc7d16*a3 - fc1d16*a0;
      PTmp[24] = fc3d16*a2 - fc5d16*a1;
      PTmp[40] = fc5d16*a2 + fc3d16*a1;
   }

   PTmp = Tmp;
   for( i=8; i>0; i--, PTmp+=8, POut+=8 )  // rows
   {
      FLT a0,a1,a2,a3;
      FLT c0,c3;

      a1 = PTmp[1] + PTmp[6];
      a2 = PTmp[2] + PTmp[5];
      FXCHG_REV(a1,a2);
      c3 = PTmp[0] - PTmp[7];
      a0 = PTmp[0] + PTmp[7];
      a3 = PTmp[3] + PTmp[4];
      c0 = PTmp[3] - PTmp[4];
      FXCHG_REV(a0,a3);
      FXCHG_REV(a0,a1);
      POut[0] = fc1d4*a0;
      POut[4] = fc1d4*a1;

      a0 = PTmp[1] - PTmp[6];
      a1 = PTmp[2] - PTmp[5];
      FXCHG_REV(a0,a1);
      POut[2] = fc3d8*a2 + fc1d8*a3;
      POut[6] = fc3d8*a3 - fc1d8*a2;
      a1 = fc1d4p*a1;
      a0 = fc1d4p*a0;
      FXCHG_REV(c0,a1); 
      FXCHG_REV(c3,a0);

      POut[1] = fc7d16*c0 + fc1d16*c3;
      POut[7] = fc7d16*c3 - fc1d16*c0;
      POut[3] = fc3d16*a0 - fc5d16*a1;
      POut[5] = fc3d16*a1 + fc5d16*a0;

   }
}

#endif	// UNIX

/*******************************************************/
