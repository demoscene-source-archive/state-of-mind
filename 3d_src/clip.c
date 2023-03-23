/***********************************************
 *              clip.c                         *
 * modified, less recursive,                   *
 * Sutherland-Hodgman clipping algorithm       *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/

static P_VERTICE Extra_Vertices[MAX_ADDITIONAL_VERTICES];
static INT Nb_Extra_Vtx;

EXTERN INT Nb_Out_Edges;
EXTERN INT Nb_Edges_To_Deal_With;
EXTERN P_VERTICE *Out_Vtx[ MAX_EDGES+1+MAX_ADDITIONAL_VERTICES];
EXTERN P_VERTICE *P_Edges[MAX_EDGES+1];
EXTERN USHORT Vtx_Flags[ MAX_EDGES+1+MAX_ADDITIONAL_VERTICES];

/******************************************************************/

#define CHECK_VTX
//#define CHECK_VTX  { if ( Nb_Extra_Vtx==MAX_ADDITIONAL_VERTICES ) abort( ); }

/******************************************************************/
/******************************************************************/

static USHORT Get_Vertex_Flags( P_VERTICE *P )
{
   USHORT State;

   State = 0x00;
   if ( P->xp<_RCst_.Clips[0] ) State |= VTX_CLIP_Xo;
   if ( P->xp>=_RCst_.Clips[1] ) State |= VTX_CLIP_X1;
   if ( P->yp<_RCst_.Clips[2] ) State |= VTX_CLIP_Yo;
   if ( P->yp>=_RCst_.Clips[3] ) State |= VTX_CLIP_Y1;
   return( State );
}

static USHORT Do_SH_Clip_Xo( )   // Clip_X left
{
   P_VERTICE *I, *Ps, *Pf;
   FLT x;
   USHORT Flags, New_Flags, All_Flags;
   INT Cur, Next;

   Cur = 0; Ps = Out_Vtx[0]; 
   Next = 1; Pf = Out_Vtx[1];
   Flags = Vtx_Flags[0] | (Vtx_Flags[1]<<8 );
   All_Flags = 0x00;
   while( Next<=Nb_Out_Edges )
   {
      switch( Flags & (VTX_CLIP_Xo | (VTX_CLIP_Xo<<8) ) )
      {
         case ( (VTX_CLIP_Xo<<8) | VTX_CLIP_Xo ): // Start=Out, Final=Out
            Ps = Pf;
            Flags >>= 8;
            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;
         break;
         case (VTX_CLIP_Xo<<8):     // Start=In, Final=Out
            I = &Extra_Vertices[Nb_Extra_Vtx++];
            CHECK_VTX
            x = ( _RCst_.Clips[0] - Ps->xp ) / ( Pf->xp - Ps->xp );
            (*Cur_Shader->Split_Edge)( x, Ps, Pf, I );
            I->xp = _RCst_.Clips[0];
            Ps = Pf;
            Flags >>= 8;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = I;
            if ( I->xp>=_RCst_.Clips[1] ) New_Flags = VTX_CLIP_X1;
            else New_Flags = 0x00;
            Vtx_Flags[Cur] = New_Flags;
            All_Flags |= New_Flags;
            Cur++;
         break;
         case VTX_CLIP_Xo:          // Start=Out, Final=In
            I = &Extra_Vertices[Nb_Extra_Vtx++];
            CHECK_VTX
            x = ( _RCst_.Clips[0] - Ps->xp ) / ( Pf->xp - Ps->xp );
            (*Cur_Shader->Split_Edge)( x, Ps, Pf, I );
            I->xp = _RCst_.Clips[0];
            Ps = Pf;
            Flags >>= 8;
            All_Flags |= Flags;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = I;
            if ( I->xp>=_RCst_.Clips[1] ) New_Flags = VTX_CLIP_X1;
            else New_Flags = 0x00;
            Vtx_Flags[Cur] = New_Flags;
            Cur++;
            Out_Vtx[Cur] = Ps;
            Vtx_Flags[Cur] = Flags&0xFF;
            Cur++;
         break;
         case 0x0000:               // Start=In, Final=In
            Ps = Pf;
            Flags >>= 8;
            All_Flags |= Flags;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = Ps;
            Vtx_Flags[Cur] = Flags&0xFF;
            Cur++;
         break;
      }
   }
   Nb_Out_Edges = Cur;
   Out_Vtx[Nb_Out_Edges] = Out_Vtx[0];
   Vtx_Flags[Nb_Out_Edges] = Vtx_Flags[0];
   return( All_Flags );
}

static USHORT Do_SH_Clip_X1( )   // Clip_X right
{
   P_VERTICE *I, *Ps, *Pf;
   FLT x;
   USHORT Flags, All_Flags;
   INT Cur, Next;

   Cur = 0; Ps = Out_Vtx[0]; 
   Next = 1; Pf = Out_Vtx[1];
   Flags = Vtx_Flags[0] | (Vtx_Flags[1]<<8 );
   All_Flags = 0x00;
   while( Next<=Nb_Out_Edges )
   {
      switch( Flags & (VTX_CLIP_X1 | (VTX_CLIP_X1<<8) ) )
      {
         case ( (VTX_CLIP_X1<<8) | VTX_CLIP_X1 ): // Start=Out, Final=Out
            Ps = Pf;
            Flags >>= 8;
            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;
         break;
         case (VTX_CLIP_X1<<8): // Start=In, Final=Out
            I = &Extra_Vertices[Nb_Extra_Vtx++];
            CHECK_VTX
            x = ( _RCst_.Clips[1] - Ps->xp ) / ( Pf->xp - Ps->xp );
            (*Cur_Shader->Split_Edge)( x, Ps, Pf, I );
            I->xp = _RCst_.Clips[1];
            Ps = Pf;
            Flags >>= 8;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = I;
            Vtx_Flags[Cur] = 0x00;
            Cur++;
         break;
         case VTX_CLIP_X1:       // Start=Out, Final=In
            I = &Extra_Vertices[Nb_Extra_Vtx++];
            CHECK_VTX
            x = ( _RCst_.Clips[1] - Ps->xp ) / ( Pf->xp - Ps->xp );
            (*Cur_Shader->Split_Edge)( x, Ps, Pf, I );
            I->xp = _RCst_.Clips[1];
            Ps = Pf;
            Flags >>= 8;
            All_Flags |= Flags;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = I;
            Vtx_Flags[Cur] = 0x00;
            Cur++;

            Out_Vtx[Cur] = Ps;
            Vtx_Flags[Cur] = Flags&0xFF;
            Cur++;
         break;
         case 0x0000:            // Start=In, Final=In
            Ps = Pf;
            Flags >>= 8;
            All_Flags |= Flags;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = Ps;
            Vtx_Flags[Cur] = Flags&0xFF;
            Cur++;
         break;
      }
   }
   Nb_Out_Edges = Cur;
   Out_Vtx[Nb_Out_Edges] = Out_Vtx[0];
   Vtx_Flags[Nb_Out_Edges] = Vtx_Flags[0];
   return( All_Flags );
}

static USHORT Do_SH_Clip_Zo( )      // Clip_Z front
{
   P_VERTICE *I, *Ps, *Pf;
   FLT x;
   USHORT Flags, New_Flags, All_Flags;
   INT Cur, Next;

   Cur = 0; Ps = Out_Vtx[0]; 
   Next = 1; Pf = Out_Vtx[1];
   Flags = Vtx_Flags[0] | (Vtx_Flags[1]<<8 );
   All_Flags = 0x00;
   while( Next<=Nb_Out_Edges )
   {
      switch( Flags & (VTX_CLIP_Zo | (VTX_CLIP_Zo<<8) ) )
      {
         case ( (VTX_CLIP_Zo<<8) | VTX_CLIP_Zo ): // Start=Out, Final=Out
            Ps = Pf;
            Flags >>= 8;
            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;
         break;
         case (VTX_CLIP_Zo<<8):     // Start=In, Final=Out 
            I = &Extra_Vertices[Nb_Extra_Vtx++];
            CHECK_VTX
            x = ( _RCst_.Clips[5] - Ps->Inv_Z ) / ( Pf->Inv_Z - Ps->Inv_Z );
            (*Cur_Shader->Split_Edge)( x, Ps, Pf, I );
//            I->N[2] = _RCst_.Clips[4];
//            I->Inv_Z = _RCst_.Clips[5];
            Ps = Pf;
            Flags >>= 8;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = I;
            New_Flags = Get_Vertex_Flags( I );
            Vtx_Flags[Cur] = New_Flags;
            All_Flags |= New_Flags;
            Cur++;
         break;
         case VTX_CLIP_Zo:          // Start=Out, Final=In 
            I = &Extra_Vertices[Nb_Extra_Vtx++];
            CHECK_VTX
            x = ( _RCst_.Clips[5] - Ps->Inv_Z ) / ( Pf->Inv_Z - Ps->Inv_Z );
            (*Cur_Shader->Split_Edge)( x, Ps, Pf, I );
//            I->N[2] = _RCst_.Clips[4];
//            I->Inv_Z = _RCst_.Clips[5];
            New_Flags = Get_Vertex_Flags( I );
            Ps = Pf;
            Flags >>= 8;
            All_Flags |= Flags;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = I;
            Vtx_Flags[Cur] = New_Flags;
            Cur++;
            All_Flags |= New_Flags;

            Out_Vtx[Cur] = Ps;
            Vtx_Flags[Cur] = Flags&0xFF;
            Cur++;
         break;
         case 0x0000:               // Start=In, Final=In 
            Ps = Pf;
            Flags >>= 8;
            All_Flags |= Flags;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = Ps;
            Vtx_Flags[Cur] = Flags&0xFF;
            Cur++;
         break;
      }
   }
   Nb_Out_Edges = Cur;
   Out_Vtx[Nb_Out_Edges] = Out_Vtx[0];
   Vtx_Flags[Nb_Out_Edges] = Vtx_Flags[0];
   return( All_Flags );
}

static USHORT Do_SH_Clip_Z1( )      // Clip_Z back
{
   P_VERTICE *I, *Ps, *Pf;
   FLT x;
   USHORT Flags, New_Flags, All_Flags;
   INT Cur, Next;

   Cur = 0; Ps = Out_Vtx[0]; 
   Next = 1; Pf = Out_Vtx[1];
   Flags = Vtx_Flags[0] | (Vtx_Flags[1]<<8 );
   All_Flags = 0x00;
   while( Next<=Nb_Out_Edges )
   {
      switch( Flags & (VTX_CLIP_Z1 | (VTX_CLIP_Z1<<8) ) )
      {
         case ( (VTX_CLIP_Z1<<8) | VTX_CLIP_Z1 ): // Start=Out, Final=Out
            Ps = Pf;
            Flags >>= 8;
            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;
         break;
         case (VTX_CLIP_Z1<<8):     // Start=In, Final=Out 
            I = &Extra_Vertices[Nb_Extra_Vtx++];
            CHECK_VTX
            x = ( _RCst_.Clips[7] - Ps->Inv_Z ) / ( Pf->Inv_Z - Ps->Inv_Z );
            (*Cur_Shader->Split_Edge)( x, Ps, Pf, I );
            Ps = Pf;
            Flags >>= 8;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = I;
            New_Flags = Get_Vertex_Flags( I );
            Vtx_Flags[Cur] = New_Flags;
            All_Flags |= New_Flags;
            Cur++;
         break;
         case VTX_CLIP_Z1:          // Start=Out, Final=In 
            I = &Extra_Vertices[Nb_Extra_Vtx++];
            
            x = ( _RCst_.Clips[7] - Ps->Inv_Z ) / ( Pf->Inv_Z - Ps->Inv_Z );
            (*Cur_Shader->Split_Edge)( x, Ps, Pf, I );
            New_Flags = Get_Vertex_Flags( I );
            Ps = Pf;
            Flags >>= 8;
            All_Flags |= Flags;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = I;
            Vtx_Flags[Cur] = New_Flags;
            Cur++;
            All_Flags |= New_Flags;

            Out_Vtx[Cur] = Ps;
            Vtx_Flags[Cur] = Flags&0xFF;
            Cur++;
         break;
         case 0x0000:               // Start=In, Final=In 
            Ps = Pf;
            Flags >>= 8;
            All_Flags |= Flags;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = Ps;
            Vtx_Flags[Cur] = Flags&0xFF;
            Cur++;
         break;
      }
   }
   Nb_Out_Edges = Cur;
   Out_Vtx[Nb_Out_Edges] = Out_Vtx[0];
   Vtx_Flags[Nb_Out_Edges] = Vtx_Flags[0];
   return( All_Flags );
}

#if 0
EXTERN void Print_Vtx( )
{
   INT j;
   for( j=0; j<Nb_Out_Edges; ++j )
      fprintf( stderr, " #%d (f=0x%.2x):  x,y=%.2f,%.2f  Inv_Z=%.2f   N=%.2f %.2f %.2f\n",
         j, Vtx_Flags[j],
         Out_Vtx[j]->xp, Out_Vtx[j]->yp,  Out_Vtx[j]->Inv_Z,
         Out_Vtx[j]->N[0], Out_Vtx[j]->N[1], Out_Vtx[j]->N[2] );
}
#endif

EXTERN void SH_Clip( USHORT All_Flags )
{
   Nb_Extra_Vtx = 0;

   if ( All_Flags & VTX_CLIP_Z1 ) All_Flags = Do_SH_Clip_Z1( );
   if ( All_Flags & VTX_CLIP_Zo ) All_Flags = Do_SH_Clip_Zo( );
   if ( All_Flags & VTX_CLIP_Xo ) All_Flags = Do_SH_Clip_Xo( );
   if ( All_Flags & VTX_CLIP_X1 ) All_Flags = Do_SH_Clip_X1( );
   // printf( "Out:%d Xtra:%d   ", Nb_Out_Edges, Nb_Extra_Vtx );
}

/*********************************************************************/
/*********************************************************************/

static USHORT Do_SH_Clip_Yo( )      // Clip_Y up
{
   P_VERTICE *I, *Ps, *Pf;
   FLT x;
   USHORT Flags, New_Flags, All_Flags;
   INT Cur, Next;

   Cur = 0; Ps = Out_Vtx[0]; 
   Next = 1; Pf = Out_Vtx[1];
   Flags = Vtx_Flags[0] | (Vtx_Flags[1]<<8 );
   All_Flags = 0x00;
   while( Next<=Nb_Out_Edges )
   {
      switch( Flags & (VTX_CLIP_Yo | (VTX_CLIP_Yo<<8) ) )
      {
         case ( (VTX_CLIP_Yo<<8) | VTX_CLIP_Yo ): // Start=Out, Final=Out
            Ps = Pf;
            Flags >>= 8;
            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;
         break;
         case (VTX_CLIP_Yo<<8):     // Start=In, Final=Out 
            I = &Extra_Vertices[Nb_Extra_Vtx++];
            CHECK_VTX
            x = ( _RCst_.Clips[2] - Ps->yp ) / ( Pf->yp - Ps->yp );
            (*Cur_Shader->Split_Edge)( x, Ps, Pf, I );
            I->yp = _RCst_.Clips[2];
            Ps = Pf;
            Flags >>= 8;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = I;
            New_Flags = Get_Vertex_Flags( I );
            Vtx_Flags[Cur] = New_Flags;
            All_Flags |= New_Flags;
            Cur++;
         break;
         case VTX_CLIP_Yo:          // Start=Out, Final=In 
            I = &Extra_Vertices[Nb_Extra_Vtx++];
            CHECK_VTX
            x = ( _RCst_.Clips[2] - Ps->yp ) / ( Pf->yp - Ps->yp );
            (*Cur_Shader->Split_Edge)( x, Ps, Pf, I );
            I->yp = _RCst_.Clips[2];
            New_Flags = Get_Vertex_Flags( I );
            Ps = Pf;
            Flags >>= 8;
            All_Flags |= Flags;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = I;
            Vtx_Flags[Cur] = New_Flags;
            Cur++;
            All_Flags |= New_Flags;

            Out_Vtx[Cur] = Ps;
            Vtx_Flags[Cur] = Flags&0xFF;
            Cur++;
         break;
         case 0x0000:               // Start=In, Final=In 
            Ps = Pf;
            Flags >>= 8;
            All_Flags |= Flags;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = Ps;
            Vtx_Flags[Cur] = Flags&0xFF;
            Cur++;
         break;
      }
   }
   Nb_Out_Edges = Cur;
   Out_Vtx[Nb_Out_Edges] = Out_Vtx[0];
   Vtx_Flags[Nb_Out_Edges] = Vtx_Flags[0];
   return( All_Flags );
}

static USHORT Do_SH_Clip_Y1( )      // Clip_Y down
{
   P_VERTICE *I, *Ps, *Pf;
   FLT x;
   USHORT Flags, New_Flags, All_Flags;
   INT Cur, Next;

   Cur = 0; Ps = Out_Vtx[0]; 
   Next = 1; Pf = Out_Vtx[1];
   Flags = Vtx_Flags[0] | (Vtx_Flags[1]<<8 );
   All_Flags = 0x00;
   while( Next<=Nb_Out_Edges )
   {
      switch( Flags & (VTX_CLIP_Y1 | (VTX_CLIP_Y1<<8) ) )
      {
         case ( (VTX_CLIP_Y1<<8) | VTX_CLIP_Y1 ): // Start=Out, Final=Out
            Ps = Pf;
            Flags >>= 8;
            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;
         break;
         case (VTX_CLIP_Y1<<8):     // Start=In, Final=Out 
            I = &Extra_Vertices[Nb_Extra_Vtx++];
            CHECK_VTX
            x = ( _RCst_.Clips[3] - Ps->yp ) / ( Pf->yp - Ps->yp );
            (*Cur_Shader->Split_Edge)( x, Ps, Pf, I );
            I->yp = _RCst_.Clips[3];
            Ps = Pf;
            Flags >>= 8;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = I;
            New_Flags = Get_Vertex_Flags( I );
            Vtx_Flags[Cur] = New_Flags;
            All_Flags |= New_Flags;
            Cur++;
         break;
         case VTX_CLIP_Y1:          // Start=Out, Final=In 
            I = &Extra_Vertices[Nb_Extra_Vtx++];
            CHECK_VTX
            x = ( _RCst_.Clips[3] - Ps->yp ) / ( Pf->yp - Ps->yp );
            (*Cur_Shader->Split_Edge)( x, Ps, Pf, I );
            I->yp = _RCst_.Clips[3];
            New_Flags = Get_Vertex_Flags( I );
            Ps = Pf;
            Flags >>= 8;
            All_Flags |= Flags;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = I;
            Vtx_Flags[Cur] = New_Flags;
            Cur++;
            All_Flags |= New_Flags;

            Out_Vtx[Cur] = Ps;
            Vtx_Flags[Cur] = Flags&0xFF;
            Cur++;
         break;
         case 0x0000:               // Start=In, Final=In 
            Ps = Pf;
            Flags >>= 8;
            All_Flags |= Flags;

            ++Next;
            Pf = Out_Vtx[Next];
            Flags |= Vtx_Flags[Next]<<8;

            Out_Vtx[Cur] = Ps;
            Vtx_Flags[Cur] = Flags&0xFF;
            Cur++;
         break;
      }
   }
   Nb_Out_Edges = Cur;
   Out_Vtx[Nb_Out_Edges] = Out_Vtx[0];
   Vtx_Flags[Nb_Out_Edges] = Vtx_Flags[0];
   return( All_Flags );
}

EXTERN void SH_Clip_Full( USHORT All_Flags )
{
   Nb_Extra_Vtx = 0;

   if ( All_Flags & VTX_CLIP_Z1 ) All_Flags = Do_SH_Clip_Z1( );
   if ( All_Flags & VTX_CLIP_Zo ) All_Flags = Do_SH_Clip_Zo( );
   if ( All_Flags & VTX_CLIP_Yo ) All_Flags = Do_SH_Clip_Yo( );
   if ( All_Flags & VTX_CLIP_Y1 ) All_Flags = Do_SH_Clip_Y1( );
   if ( All_Flags & VTX_CLIP_Xo ) All_Flags = Do_SH_Clip_Xo( );
   if ( All_Flags & VTX_CLIP_X1 ) All_Flags = Do_SH_Clip_X1( );
   // printf( "Out:%d Xtra:%d   ", Nb_Out_Edges, Nb_Extra_Vtx );
}


/*********************************************************************/
/*********************************************************************/
/*  Clipping methods. Warning: *Result CAN be equal to *To or *From  */
/*********************************************************************/
/*********************************************************************/

EXTERN void Split_UV_Gouraud(
   FLT x, P_VERTICE *From, P_VERTICE *To, P_VERTICE *Result )
{
   Result->Inv_Z = From->Inv_Z + x*( To->Inv_Z-From->Inv_Z );
   Result->N[2] = 1.0f/Result->Inv_Z;
   Result->N[0] = From->N[0] + x*( To->N[0] - From->N[0] );
   Result->UV[0] = From->UV[0] + x*( To->UV[0] - From->UV[0] );
   Result->UV[1] = From->UV[1] + x*( To->UV[1] - From->UV[1] );
   Result->xp = From->xp + x*( To->xp - From->xp );
   Result->yp = From->yp + x*( To->yp - From->yp );
}

EXTERN void Split_UV(
   FLT x, P_VERTICE *From, P_VERTICE *To, P_VERTICE *Result )
{
   Result->Inv_Z = From->Inv_Z + x*( To->Inv_Z-From->Inv_Z );
   Result->N[2] = 1.0f/Result->Inv_Z;
   Result->UV[0] = From->UV[0] + x*( To->UV[0] - From->UV[0] );
   Result->UV[1] = From->UV[1] + x*( To->UV[1] - From->UV[1] );
   Result->xp = From->xp + x*( To->xp - From->xp );
   Result->yp = From->yp + x*( To->yp - From->yp );
}

#if 0
EXTERN void Split_UV_Gouraud2(
   FLT x, P_VERTICE *From, P_VERTICE *To, P_VERTICE *Result )
{
   Result->Inv_Z = From->Inv_Z + x*( To->Inv_Z-From->Inv_Z );
   Result->N[2] = 1.0/Result->Inv_Z;
   Result->N[0] = From->N[0] + x*( To->N[0]-From->N[0] );
   Result->UV[0] = From->UV[0] + x*( To->UV[0] - From->UV[0] );
   Result->UV[1] = From->UV[1] + x*( To->UV[1] - From->UV[1] );
   Result->xp = From->xp + x*( To->xp - From->xp );
   Result->yp = From->yp + x*( To->yp - From->yp );
}
#endif

EXTERN void Split_Raw(
   FLT x, P_VERTICE *From, P_VERTICE *To, P_VERTICE *Result )
{
   Result->Inv_Z = From->Inv_Z + x*( To->Inv_Z-From->Inv_Z );
   Result->N[2] = 1.0f/Result->Inv_Z;
   Result->xp = From->xp + x*( To->xp - From->xp );
   Result->yp = From->yp + x*( To->yp - From->yp );
}

EXTERN void Split_Gouraud(
   FLT x, P_VERTICE *From, P_VERTICE *To, P_VERTICE *Result )
{
   Result->Inv_Z = From->Inv_Z + x*( To->Inv_Z-From->Inv_Z );
   Result->N[2] = 1.0f/Result->Inv_Z;
   Result->N[0] = From->N[0] + x*( To->N[0]-From->N[0] );
   Result->xp = From->xp + x*( To->xp - From->xp );
   Result->yp = From->yp + x*( To->yp - From->yp );
}

EXTERN void Split_Shadow(
   FLT x, P_VERTICE *From, P_VERTICE *To, P_VERTICE *Result )
{
   Result->Inv_Z = From->Inv_Z + x*( To->Inv_Z-From->Inv_Z );
   Result->N[2] = 1.0f/Result->Inv_Z;
   Result->N[0] = From->N[0] + x*( To->N[0]-From->N[0] );
   Result->N[1] = From->N[1] + x*( To->N[1]-From->N[1] );
   Result->xp = From->xp + x*( To->xp - From->xp );
   Result->yp = From->yp + x*( To->yp - From->yp );
}

EXTERN void Split_Shadow_UV(
   FLT x, P_VERTICE *From, P_VERTICE *To, P_VERTICE *Result )
{
   Result->Inv_Z = From->Inv_Z + x*( To->Inv_Z-From->Inv_Z );
   Result->N[2] = 1.0f/Result->Inv_Z;
   Result->N[0] = From->N[0] + x*( To->N[0]-From->N[0] );
   Result->N[1] = From->N[1] + x*( To->N[1]-From->N[1] );
   Result->UV[0] = From->UV[0] + x*( To->UV[0] - From->UV[0] );
   Result->UV[1] = From->UV[1] + x*( To->UV[1] - From->UV[1] );
   Result->xp = From->xp + x*( To->xp - From->xp );
   Result->yp = From->yp + x*( To->yp - From->yp );
}

EXTERN void Split_UVc(
   FLT x, P_VERTICE *From, P_VERTICE *To, P_VERTICE *Result )
{
   FLT Tmp, x1, x2;

   Result->Inv_Z = From->Inv_Z + x*( To->Inv_Z-From->Inv_Z );
   Tmp = Result->N[2] = 1.0f/Result->Inv_Z;
   x2 = To->UV[0]*To->Inv_Z;
   x1 = From->UV[0]*From->Inv_Z;
   Result->UV[0] = ( x1 + x*( x2-x1 ) ) * Tmp;
   x2 = To->UV[1]*To->Inv_Z;
   x1 = From->UV[1]*From->Inv_Z;
   Result->UV[1] = ( x1 + x*( x2-x1 ) ) * Tmp;
   Result->xp = From->xp + x*( To->xp - From->xp );
   Result->yp = From->yp + x*( To->yp - From->yp );
}

EXTERN void Split_UVc_Gouraud(
   FLT x, P_VERTICE *From, P_VERTICE *To, P_VERTICE *Result )
{
   FLT Tmp, x1, x2;

   Result->Inv_Z = From->Inv_Z + x*( To->Inv_Z-From->Inv_Z );
   Tmp = Result->N[2] = 1.0f/Result->Inv_Z;
   x2 = To->UV[0]*To->Inv_Z;
   x1 = From->UV[0]*From->Inv_Z;
   Result->UV[0] = ( x1 + x*( x2-x1 ) ) * Tmp;
   x2 = To->UV[1]*To->Inv_Z;
   x1 = From->UV[1]*From->Inv_Z;
   Result->UV[1] = ( x1 + x*( x2-x1 ) ) * Tmp;

   Result->N[0] = From->N[0] + x*( To->N[0] - From->N[0] );
   Result->xp = From->xp + x*( To->xp - From->xp );
   Result->yp = From->yp + x*( To->yp - From->yp );
}

/******************************************************************/

#if 0
EXTERN void Split_Raw_Z(
   FLT x, P_VERTICE *From, P_VERTICE *To, P_VERTICE *Result )
{
   Result->Inv_Z = To->Inv_Z*From->Inv_Z / ( From->Inv_Z + x*( To->Inv_Z-From->Inv_Z ) );
   Result->xp = From->xp + x*( To->xp-From->xp );
   Result->yp = From->yp + x*( To->yp-From->yp );
}

EXTERN void Split_UV_Z(
   FLT x, P_VERTICE *From, P_VERTICE *To, P_VERTICE *Result )
{
   Result->Inv_Z = To->Inv_Z*From->Inv_Z / ( From->Inv_Z + x*( To->Inv_Z-From->Inv_Z ) );
   Result->UV[0] = From->UV[0] + x*( To->UV[0]-From->UV[0] );
   Result->UV[1] = From->UV[1] + x*( To->UV[1]-From->UV[1] );
   Result->xp = From->xp + x*( To->xp-From->xp );
   Result->yp = From->yp + x*( To->yp-From->yp );
}
#endif

/******************************************************************/
/******************************************************************/
