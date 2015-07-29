#include "include/Serializer.h"

#include <string.h> 		/* strchr,strlen */
#include <stdlib.h>		/* exit */

#define GT_SER_KEY_P			(char)0x10
#define GT_SER_KEY_L			(char)0x20
#define GT_SER_KEY_A			(char)0x30
#define GT_SER_KEY_SegmentList2	(char)0x40
#define GT_SER_KEY_Region2		(char)0x50
#define GT_SER_KEY_Set2		(char)0x50

#define GT_SER_TEXT_SegmentList2	"seglist2"
#define GT_SER_TEXT_Region2		"region2"
#define GT_SER_TEXT_Set2		"set2"

#define GT_SER_PRECISION 8 
#define GT_SER_FLOAT_FORMAT "%.8f"
#define GT_SER_FLOAT_READ "%lf"
#define GT_SER_FLOAT_CHARS (9+GT_SER_PRECISION) 
#define GT_SER_MAXSEGELEMENTS 10

#define GT_SER_BUFFER_DEATH { printf("SERIALIZER BUFFER OVERRUN"); exit(3); }

int internal_splitHelper(char * input, char ** pieces, const char splitter, const int maxSplits)
{
  int i, out_i, len;
  out_i = 1;
  len = strlen(input);
  
  pieces[0] = input;
  for (i = 1; i < len && out_i < maxSplits; ++i)
    {
      if (input[i] == splitter)
	pieces[out_i++] = input + i;
    }
  if (out_i < maxSplits)
    pieces[out_i] = NULL;

  return (out_i);
}
char * strnchr(char * str, const char character) /* this seems dumb, why am I doing this? */
{
  int i = 0;
  while (str[i] != 0 && str[i] == character)
    ++i;

  return(str+i);
}

int Serialize_P(const SerializeMode mode, const Point2 * const input, char * buff, const unsigned int buffSize)
{
  int res = -2;
  GT_SERMODE_VALID(mode);
  assert(buff != NULL);
  assert(input != NULL);
  assert(buffSize > 0);

  /* just assume that the inputs are less than 8 digits. */
  assert( input->x < 99999999 && input->x > -9999999 );
  assert( input->y < 99999999 && input->y > -9999999 );

  switch (mode)
    {
    case JSON:
      /* [X,Y] number of characters is 3 + 2 {radix pts} + 2*precision + 2*size, lets just assume size <= 1000000*/
      if (buffSize > 4+2*GT_SER_FLOAT_CHARS)
	{
	  res = sprintf(buff, "[" GT_SER_FLOAT_FORMAT "," GT_SER_FLOAT_FORMAT "]", input->x, input->y);
	}
      break;
    case SEXP:
      /* (X Y) */
      if (buffSize > 4+2*GT_SER_FLOAT_CHARS)
	{
	  res = sprintf(buff, "(" GT_SER_FLOAT_FORMAT " " GT_SER_FLOAT_FORMAT ")", input->x, input->y);
	}
      break;
    case BINARY:
      /* byte array (char array)
       [ (8 bit)GT_SER_KEY_P, (64 bit?) pt x, (64 bit?) pt y]*/
      if ((unsigned int)buffSize > sizeof(gtfloat)*2 + sizeof(unsigned char))
	{			/* pack 'em in */
	  buff[0] = GT_SER_KEY_P;
	  buff[1] = input->x; /* is this even valid???? */
	  buff[1+sizeof(gtfloat)] = input->y; /* is this?!!?!? */
	  res = sizeof(gtfloat)*2 + sizeof(unsigned char);
        }
      break;
    default:
      res = -3;
    }
  
  if (res > (int)buffSize) /* you blew through your buffer */
      GT_SER_BUFFER_DEATH;
  
  return(res);
}
int Deserialize_P(const SerializeMode mode, char * input, Point2 * buff)
{
  int res = -1;
  char * blockEnd;
  GT_SERMODE_VALID(mode);
  assert(input != NULL);
  assert(buff != NULL);
  switch (mode)
    {
    case JSON:
      /* [X,Y] */
      if (sscanf(input, "[" GT_SER_FLOAT_READ "," GT_SER_FLOAT_READ "]", &(buff->x), &(buff->y)) == 2)
	{  /* it worked, now set res to be the number of chars you consumed (find first "]") */
	  blockEnd = strchr(input, ']');
	  res = blockEnd - input;
	}
      break;
    case SEXP:
      /* (X Y) */
      if (sscanf(input, "(" GT_SER_FLOAT_READ " " GT_SER_FLOAT_READ ")", &(buff->x), &(buff->y)) == 2)
	{  /* it worked, now set res to be the number of chars you consumed (find first ")") */
	  blockEnd = strchr(input, ')');
	  res = blockEnd - input;
	}
      break;
    case BINARY:
      /* byte array (char array)
       [ (8 bit)GT_SER_KEY_P, (64 bit?) pt x, (64 bit?) pt y]*/
      if (input[0] == GT_SER_KEY_P)
	{
	  buff->x = (gtfloat)input[1];
	  buff->y = (gtfloat)input[1+sizeof(gtfloat)];
	  res = 1 + 2*sizeof(gtfloat);
        }
      break;
    default:
      res = -3;
    }
  return(res);
}

int Serialize_L(const SerializeMode mode, const Line2 * const input, char * buff, const unsigned int buffSize)
{
  int res = -2;

  GT_SERMODE_VALID(mode);
  assert(buff != NULL);
  
  switch (mode)
    {
    case JSON:
      /* [sX,sY,eX,eY] number of characters is 5 + 4 {radix pts} + 4*precision + 4*size, lets just assume size <= 1000000*/
      if (buffSize > 6+4*GT_SER_FLOAT_CHARS)
	{
	  res = sprintf(buff, "[" GT_SER_FLOAT_FORMAT "," GT_SER_FLOAT_FORMAT "," GT_SER_FLOAT_FORMAT "," GT_SER_FLOAT_FORMAT "]", input->start.x, input->start.y, input->end.x, input->end.y);
	}
      break;
    case SEXP:
      /* (sX sY eX eY) */
      if (buffSize > 6 +4*GT_SER_FLOAT_CHARS)
	{
	  res = sprintf(buff, "(" GT_SER_FLOAT_FORMAT " " GT_SER_FLOAT_FORMAT " " GT_SER_FLOAT_FORMAT " " GT_SER_FLOAT_FORMAT ")", input->start.x, input->start.y, input->end.x, input->end.y);
	}
      break;
    case BINARY:
      /* byte array (char array)
       [ (8 bit)GT_SER_KEY_P, (64 bit?) pt x, (64 bit?) pt y]*/
      if ((unsigned int)buffSize > sizeof(gtfloat)*4 + sizeof(unsigned char))
	{			/* pack 'em in */
	  buff[0] = GT_SER_KEY_L;
	  memcpy(buff+1,                    &(input->start.x), sizeof(gtfloat));
	  memcpy(buff+1+1*sizeof(gtfloat), &(input->start.y), sizeof(gtfloat));
	  memcpy(buff+1+2*sizeof(gtfloat), &(input->end.x),   sizeof(gtfloat));
	  memcpy(buff+1+3*sizeof(gtfloat), &(input->end.y),   sizeof(gtfloat));
	  res = sizeof(gtfloat)*4 + sizeof(unsigned char);
        }
      break;
    default:
      res = -3;
    }
  
  if (res > (int)buffSize) /* you blew through your buffer */
      GT_SER_BUFFER_DEATH;
  
  return(res);
}
int Deserialize_L(const SerializeMode mode, char * input, Line2 * buff)
{
  int res = -1;
  char * blockEnd;
  GT_SERMODE_VALID(mode);
  assert(input != NULL);
  assert(buff != NULL);

  switch (mode)
    {
    case JSON:
      /* [sX,sY,eX,eY] */
      if (sscanf(input, "[" GT_SER_FLOAT_READ "," GT_SER_FLOAT_READ "," GT_SER_FLOAT_READ "," GT_SER_FLOAT_READ "]", &(buff->start.x), &(buff->start.y), &(buff->end.x), &(buff->end.y)) == 4)
	{  /* it worked, now set res to be the number of chars you consumed (find first "]") */
	  blockEnd = strchr(input, ']');
	  res = blockEnd - input;
	}
      break;
    case SEXP:
      /* (sX sY eX eY) */
      if (sscanf(input, "(" GT_SER_FLOAT_READ " " GT_SER_FLOAT_READ " " GT_SER_FLOAT_READ " " GT_SER_FLOAT_READ ")", &(buff->start.x), &(buff->start.y), &(buff->end.x), &(buff->end.y)) == 4)
	{  /* it worked, now set res to be the number of chars you consumed (find first ")") */
	  blockEnd = strchr(input, ')');
	  res = blockEnd - input;
	}
      break;
    case BINARY:
      /* byte array (char array)
       [ (8 bit)GT_SER_KEY_L, (64 bit?) sx, (64 bit?) sy, (64 bit?) ex, (64 bit?) ey]*/
      if (input[0] == GT_SER_KEY_L)
	{
	  memcpy(&(buff->start.x), input+1,                    sizeof(gtfloat));
	  memcpy(&(buff->start.y), input+1+1*sizeof(gtfloat), sizeof(gtfloat));
	  memcpy(&(buff->end.x),   input+1+2*sizeof(gtfloat), sizeof(gtfloat));
	  memcpy(&(buff->end.y),   input+1+3*sizeof(gtfloat), sizeof(gtfloat));
	  res = 1 + 4*sizeof(gtfloat);
        }
      break;
    default:
      res = -3;
    }
  return(res);
}

int Serialize_A(const SerializeMode mode, const Arc2 * const input, char * buff, const unsigned int buffSize)
{
  int res = -2;

  GT_SERMODE_VALID(mode);
  assert(buff != NULL);
  
  switch (mode)
    {
    case JSON:
      /* [rotDir,cX,cY,radius,startRad,endRad] 
	 number or characters 7 + 5 {radix pts} + 5*precision + 5*size + rotdir(could have "-" sign, 
	 so two chars, lets just assume size <= 1000000*/
      if (buffSize > 10+5*GT_SER_FLOAT_CHARS)
	{
	  res = sprintf(buff, "[%d," GT_SER_FLOAT_FORMAT "," GT_SER_FLOAT_FORMAT "," GT_SER_FLOAT_FORMAT "," GT_SER_FLOAT_FORMAT "," GT_SER_FLOAT_FORMAT "]", input->angle.rot, input->center.x, input->center.y, input->radius, input->angle.start, input->angle.end);
	}
      break;
    case SEXP:
      /* (rotDir, cX, cY, radius, startRad, endRad) */
      if (buffSize > 10+5*GT_SER_FLOAT_CHARS)
	{
	  res = sprintf(buff, "(%d " GT_SER_FLOAT_FORMAT " " GT_SER_FLOAT_FORMAT " " GT_SER_FLOAT_FORMAT " " GT_SER_FLOAT_FORMAT " " GT_SER_FLOAT_FORMAT ")", input->angle.rot, input->center.x, input->center.y, input->radius, input->angle.start, input->angle.end);
	}
      break;
    case BINARY:
      /* byte array (char array) 
	 [ (8 bit)GT_SER_KEY_A, (8 bit) rotDir, ptx, pty, radius, startRad, endRad] */
      if ((unsigned int)buffSize > sizeof(gtfloat)*5 + 2*sizeof(unsigned char))
	{			/* pack 'em in */
	  buff[0] = GT_SER_KEY_A;
	  buff[1] = (char)input->angle.rot;
	  buff[2] = input->center.x;
	  buff[2+sizeof(gtfloat)] = input->center.y;
	  buff[2+2*sizeof(gtfloat)] = input->radius;
	  buff[2+3*sizeof(gtfloat)] = input->angle.start;
	  buff[2+4*sizeof(gtfloat)] = input->angle.end;
	  res = sizeof(gtfloat)*5 + 2*sizeof(unsigned char);
        }
      break;
    default:
      res = -3;
    }
  
  if (res > (int)buffSize) /* you blew through your buffer */
      GT_SER_BUFFER_DEATH;
  
  return(res);
}
int Deserialize_A(const SerializeMode mode, char * input, Arc2 * buff)
{
  int res = -1;
  char * blockEnd;
  GT_SERMODE_VALID(mode);
  assert(input != NULL);
  assert(buff != NULL);

  switch (mode)
    {
    case JSON:
      /* [rotDir,cX,cY,radius,startRad,endRad] 
	 number or characters 7 + 5 {radix pts} + 5*precision + 5*size + rotdir(could have "-" sign, 
	 so two chars, lets just assume size <= 1000000*/
      if (sscanf(input, "[%hd," GT_SER_FLOAT_READ "," GT_SER_FLOAT_READ "," GT_SER_FLOAT_READ "," GT_SER_FLOAT_READ "," GT_SER_FLOAT_READ "]", &(buff->angle.rot), &(buff->center.x), &(buff->center.y), &(buff->radius), &(buff->angle.start), &(buff->angle.end)) == 6)
	{  /* it worked, now set res to be the number of chars you consumed (find first "]") */
	  blockEnd = strchr(input, ']');
	  res = blockEnd - input;
	}
      break;
    case SEXP:
      /* (rotDir cX cY radius startRad endRad)
	 number or characters 7 + 5 {radix pts} + 5*precision + 5*size + rotdir(could have "-" sign, 
	 so two chars, lets just assume size <= 1000000*/
      if (sscanf(input, "(%hd " GT_SER_FLOAT_READ " " GT_SER_FLOAT_READ " " GT_SER_FLOAT_READ " " GT_SER_FLOAT_READ " " GT_SER_FLOAT_READ ")", &(buff->angle.rot), &(buff->center.x), &(buff->center.y), &(buff->radius), &(buff->angle.start), &(buff->angle.end)) == 6)
	{  /* it worked, now set res to be the number of chars you consumed (find first "]") */
	  blockEnd = strchr(input, ')');
	  res = blockEnd - input;
	}
      break;
    case BINARY:
      /* byte array (char array) 
	 [ (8 bit)GT_SER_KEY_A, (8 bit) rotDir, ptx, pty, radius, startRad, endRad] */
      if (input[0] == GT_SER_KEY_A)
	{
	  buff->angle.rot   = (char)input[1];
	  buff->center.x    = (gtfloat)input[2];
	  buff->center.y    = (gtfloat)input[2+  sizeof(gtfloat)];
	  buff->radius      = (gtfloat)input[2+1*sizeof(gtfloat)];
	  buff->angle.start = (gtfloat)input[2+2*sizeof(gtfloat)];
	  buff->angle.end   = (gtfloat)input[2+3*sizeof(gtfloat)];
	  res = 2 + 5*sizeof(gtfloat);
        }
      break;
    default:
      res = -3;
    }
  return(res);
}

int Serialize_Seg(const SerializeMode mode, const Segment2 * const input, char * buff, const unsigned int buffSize)
{
  assert(input->type == LINE || input->type == ARC);
  if (input->type == LINE)
    return Serialize_L(mode, &(input->s.line), buff, buffSize);
  else 
    return Serialize_A(mode, &(input->s.arc) , buff, buffSize);
}
int Deserialize_Seg(const SerializeMode mode, char * input, Segment2 * output)
{
  /* returns -1 on error or number of chars read form input on success */
  /* need to figure out if this is a line or an arc.. :( */
  int numElements;
  char * elements[GT_SER_MAXSEGELEMENTS];
  char * inputStart;

  GT_SERMODE_VALID(mode);
  assert(input != NULL);
  assert(output != NULL);

  switch (mode)
    {
    case JSON:
      /* Arc : "[rotDir,cX,cY,radius,startRad,endRad]" 
	 Line: "[sX,sY,eX,eY]"
	 so I can technically count the number of elements in each array...... I think....
      */
      /* find the first non-space and non '[' character */
      inputStart = strchr(input, (int)'[');
      if (inputStart == NULL)
	{ 			/* missing an array start, drop dead */
	  return -1;
	}
            
      numElements = internal_splitHelper(inputStart, elements, ',', GT_SER_MAXSEGELEMENTS);
      if (numElements == 4) 	/* looks like a line */
	{
	  output->type = LINE;
	  return Deserialize_L(mode, input, &(output->s.line));
	}
      else if (numElements == 6) /* looks like an arc */
	{
	  output->type = ARC;
	  return Deserialize_A(mode, input, &(output->s.arc));
	}
      else 			/* looks like an error :( */
	{
	  return -1;
	}
      break;
    case SEXP:
      /* Arc : (rotDir cX cY radius startRad endRad)
	 Line: (sX sY eX eY) */      /* find the first non-space and non '[' character */
      inputStart = strchr(input, (int)'(');
      if (inputStart == NULL)
	{ 			/* missing an array start, drop dead */
	  return -1;
	}
      inputStart = strnchr(inputStart, ' ');

      numElements = internal_splitHelper(inputStart, elements, ' ', GT_SER_MAXSEGELEMENTS);
      if (numElements == 4) 	/* looks like a line */
	{
	  output->type = LINE;
	  return Deserialize_L(mode, input, &(output->s.line));
	}
      else if (numElements == 6) /* looks like an arc */
	{
	  output->type = ARC;
	  return Deserialize_A(mode, input, &(output->s.arc));
	}
      else 			/* looks like an error :( */
	{
	  return -1;
	}
      break;
    case BINARY:
      /* if check the first byte */
      if (input[0] == GT_SER_KEY_L)
	{
	  output->type = LINE;
	  return Deserialize_L(mode, input, &(output->s.line));
	}
      else if (input[0] == GT_SER_KEY_A)
	{
	  output->type = ARC;
	  return Deserialize_A(mode, input, &(output->s.arc));
	}
      else
	{
	  return -1;
	}
      break;
    default:
      return -3;
    }
}

int Serialize_SL_buffMax(const SerializeMode mode, const SegmentList2 * input)
{
  /* TODO: fix this insanely high limit to be less stupid */
  return ( 40 + 50*SegmentList2_numSegs(input) ); /* obscenely high limit */
  /* else if (mode ==  */
}
int Serialize_SL(const SerializeMode mode, const SegmentList2 * const input, char * buff, const unsigned int buffSize)
{
  int offset = 0;
  int res = -2;
  unsigned int i;

  GT_SEGLIST_VALID1(input);
  
  switch (mode)
    {
    case JSON:
      /* {"type":"seglist2","segs":[ ... segments ...]} */
      buff = "{\"type\":\"" GT_SER_TEXT_SegmentList2 "\",\"segs\":[";
      offset = strlen(buff);

      for (i = 0; i < SegmentList2_numSegs(input) && (unsigned int)offset < buffSize; ++i)
	{
	  assert(SegmentList2_seg(input,i).type == LINE || SegmentList2_seg(input,i).type == ARC);

	  if (i != 0)
	    buff[offset++] = ',';

	  if (SegmentList2_seg(input,i).type == LINE)
	    res = Serialize_L(mode, &(SegmentList2_seg(input,i).s.line), buff+offset, buffSize-offset);
	  else 			/* if(input->segments[i].type == ARC */
	    res = Serialize_A(mode, &(SegmentList2_seg(input,i).s.arc) , buff+offset, buffSize-offset);
	  
	  if (res < 0)
	    break; 		/* ERROR, get outta here */
	  offset += res;
	}
      /* if (res >= 0) */
	{
	  buff[offset++] = ']';
	  buff[offset++] = '}';
	  buff[offset] = 0;	/* null terminate the string */
	  res = offset;
	}
      break;
    case SEXP:
      /* ('seglist2 (seg) (seg) (seg)) */
      strcpy(buff, "('" GT_SER_TEXT_SegmentList2);
      offset = strlen(buff);

      for (i = 0; i < SegmentList2_numSegs(input) && (unsigned int)offset < buffSize; ++i)
	{
	  assert(SegmentList2_seg(input,i).type == LINE || SegmentList2_seg(input,i).type == ARC);

	  buff[offset++] = ' ';

	  if (SegmentList2_seg(input,i).type == LINE)
	    res = Serialize_L(mode, &(SegmentList2_seg(input,i).s.line), buff+offset, buffSize-offset);
	  else 			/* if(input->segments[i].type == ARC */
	    res = Serialize_A(mode, &(SegmentList2_seg(input,i).s.arc) , buff+offset, buffSize-offset);

	  if (res < 0)
	    break; 		/* ERROR, get outta here */
	  else
	    offset += res;
	}
      /* if (res >= 0) */
	{
	  buff[offset++] = ')';
	  buff[offset] = 0;	/* null terminate the string */
	  res = offset;
	}
      break;
    case BINARY:
      /* byte array (char array)
       [ (8 bit)GT_SER_KEY_SegmentList2, (int) type, (int) num segments, seg, seg, seg, seg,...]*/
      buff[0] = GT_SER_KEY_SegmentList2;
      /* int what = sizeof(input->private_type); */
      memcpy(buff+1,&(input->private_type),sizeof(input->private_type));
      /* SimpleRegion2Type test; */
      /* memcpy(&test, buff+1, sizeof(test)); */
      memcpy(buff+1+sizeof(input->private_type), &(SegmentList2_numSegs(input)), sizeof(SegmentList2_numSegs(input)));
      /* what+= 10; */

      offset += 1+sizeof(input->private_type)+sizeof(SegmentList2_numSegs(input));
      res = 0;

      for (i = 0; i < SegmentList2_numSegs(input) && (unsigned int)offset < buffSize; ++i)
	{
	  if (SegmentList2_seg(input,i).type == LINE)
	    res = Serialize_L(mode, &(SegmentList2_seg(input,i).s.line), buff+offset, buffSize-offset);
	  else 			/* if(input->segments[i].type == ARC */
	    res = Serialize_A(mode, &(SegmentList2_seg(input,i).s.arc) , buff+offset, buffSize-offset);
	  
	  if (res < 0)
	    break; 		/* ERROR, get outta here */
	  
	  offset += res;
	}
      if (res >= 0)
	  res = offset;

      break;
    default:
      res = -3;
    }
  
  if (res > (int)buffSize) /* you blew through your buffer */
      GT_SER_BUFFER_DEATH;
  
  return (res);
}
int Deserialize_SL(const SerializeMode mode, char * input, SegmentList2 * buff)
{
  /* returns -1 on failure, size of byte read from buff otherwise */
  int res = -1;
  unsigned int offset, i;
  /* char * blockEnd; */

  assert(input != NULL);
  GT_SEGLIST_VALID1(buff);

  SegmentList2_clear(buff);
 
  switch(mode)
    {
    case JSON:
      /* {"type":"seglist2","segs":[ ... segments ...]} */
      
      break;
    case SEXP:
      /* ('seglist2 (seg) (seg) (seg)) */
      
      break;
    case BINARY:
      /* byte array (char array)
	 [ (8 bit)GT_SER_KEY_SegmentList2, (int) type, (int) num segments, seg, seg, seg, seg,...]*/
      
      if (input[0] != GT_SER_KEY_SegmentList2)
	return -1; 		/* apparently this isn't a segment list? */

      memcpy(&(buff->private_type), input+1, sizeof(buff->private_type));
      memcpy(&(SegmentList2_numSegs(buff)), input+1+sizeof(buff->private_type), sizeof(SegmentList2_numSegs(buff)));

      offset = 1 + sizeof(buff->private_type) + sizeof(SegmentList2_numSegs(buff));
      /* ok, apparently now I just go grab all the segments? */
      for (i = 0; i < SegmentList2_numSegs(buff); ++i)
	{
	  res = Deserialize_Seg(mode, input+offset, &(SegmentList2_seg(buff,i)));
	  if (res < 0)
	    return (res);	/* failed somehow, get out of here */
	     
	  offset += (unsigned int)res;
	}
      /* thats it? I'm done? */
      res = (int)offset;
      
      break;
    default:
      break;
    }
  return (res);
}

int Serialize_R_buffMax(const SerializeMode mode, const Region2 * input)
{
  unsigned int i;
  int size;   

  size = sizeof(Region2)*20;
  for (i = 0; i < Region2_numLists(input); ++i)
    size += Serialize_SL_buffMax(mode, &(Region2_list(input,i))) + 10;
  
  return (size);
}
int Serialize_R(const SerializeMode mode, const Region2 * const input, char * buff, const unsigned int buffSize)
{
  int offset = 0;
  int res = -2;
  unsigned int i;

  GT_REGION2_VALID(input);
  
  switch (mode)
    {
    case JSON:
      /* {"type":"seglist2set","lists":[ ... lists ...]} */
      buff = "{\"type\":\"" GT_SER_TEXT_Region2 "\",\"lists\":[";
      offset = strlen(buff);

      for (i = 0; i < Region2_numLists(input) && (unsigned int)offset < buffSize; ++i)
	{
	  if (i != 0)
	    buff[offset++] = ',';

	  res = Serialize_SL(mode, &(Region2_list(input,i)), buff+offset, buffSize-offset);
	  
	  if (res < 0)
	    break; 		/* ERROR, get outta here */

	  offset += res;
	}
      /* if (res >= 0) */
	{
	  buff[offset++] = ']';
	  buff[offset++] = '}';
	  buff[offset] = 0;	/* null terminate the string */
	  res = offset;
	}
      break;
    case SEXP:
      /* ('seglist2set (seglist) (seglist) (seglist)) */
      strcpy(buff, "('" GT_SER_TEXT_Region2);
      offset = strlen(buff);

      for (i = 0; i < Region2_numLists(input) && (unsigned int)offset < buffSize; ++i)
	{
	  buff[offset++] = ' ';

	  res = Serialize_SL(mode, &(Region2_list(input,i)), buff+offset, buffSize-offset);
	  
	  if (res < 0)
	    break; 		/* ERROR, get outta here */

	  offset += res;
	}
      /* if (res >= 0) */
	{
	  buff[offset++] = ')';
	  buff[offset] = 0;	/* null terminate the string */
	  res = offset;
	}
      break;
    case BINARY:
      /* byte array (char array)
       [ (8 bit)GT_SER_KEY_Region2, (unsigned int) numLists, seg, seg, seg, seg,...]*/
      buff[0] = GT_SER_KEY_Region2;
      memcpy(buff+1,&(Region2_numLists(input)),sizeof(Region2_numLists(input)));
      offset += 1+sizeof(Region2_numLists(input));
      res = 0;

      for (i = 0; i < Region2_numLists(input) && (unsigned int)offset < buffSize; ++i)
	{
	  res = Serialize_SL(mode, &(Region2_list(input,i)), buff+offset, buffSize-offset);
	  
	  if (res < 0)
	    break; 		/* ERROR, get outta here */
	  
	  offset += res;
	}
      if (res >= 0)
	  res = offset;

      break;
    default:
      res = -3;
    }
  
  if (res > (int)buffSize) /* you blew through your buffer */
      GT_SER_BUFFER_DEATH;
  
  return (res);
}
int Deserialize_R(const SerializeMode mode, char * input, Region2 * output)
{
  /* returns -1 on failure, size of byte read from buff otherwise */
  int res = -1;
  unsigned int offset, i;
  /* char * blockEnd; */

  assert(input != NULL);
  GT_REGION2_VALID(output);

  Region2_clear(output);
 
  switch(mode)
    {
    case JSON:
      /* {"type":"seglist2set","lists":[ ... lists ...]} */
      
      break;
    case SEXP:
      /* ('seglist2list (list) (list) (list)) */
      
      break;
    case BINARY:
      /* byte array (char array)
	 [ (8 bit)GT_SER_KEY_Region2, (unsigned int) numList, list, list, list, list,...]*/
      
      if (input[0] != GT_SER_KEY_Region2)
	return -1; 		/* apparently this isn't a segment list? */

      memcpy(&(Region2_numLists(output)), input+1, sizeof(Region2_numLists(output)));

      offset = 1 + sizeof(Region2_numLists(output));
      /* ok, apparently now I just go grab all the segments? */
      for (i = 0; i < Region2_numLists(output); ++i)
	{
	  res = Deserialize_SL(mode, input+offset, &(Region2_list(output,i)));
	  if (res < 0)
	    return (res);	/* failed somehow, get out of here */
	     
	  offset += (unsigned int)res;
	}
      /* thats it? I'm done? */
      res = (int)offset;
      
      break;
    default:
      break;
    }
  return (res);
}

int Serialize_S_buffMax(const SerializeMode mode, const Set2 * input)
{
  unsigned int i;
  int size;   

  size = sizeof(Set2)*20;
  for (i = 0; i < Set2_numRegions(input); ++i)
    size += Serialize_R_buffMax(mode, &(Set2_region(input,i))) + 10;
  
  return (size);
}
int Serialize_S(const SerializeMode mode, const Set2 * const input, char * buff, const unsigned int buffSize)
{
  int offset = 0;
  int res = -2;
  unsigned int i;
  
  GT_SET2_VALID(input);
  
  switch (mode)
    {
    case JSON:
      assert(0); /* untested */
      /* {"type":"set2","regions":[ ... regions ...]} */
      buff = "{\"type\":\"" GT_SER_TEXT_Set2 "\",\"regions\":[";
      offset = strlen(buff);
      
      for (i = 0; i < Set2_numRegions(input) && (unsigned int)offset < buffSize; ++i)
	{
	  if (i != 0)
	    buff[offset++] = ',';

	  res = Serialize_R(mode, &(Set2_region(input,i)), buff+offset, buffSize-offset);
	  
	  if (res < 0)
	    break; 		/* ERROR, get outta here */

	  offset += res;
	}
      /* if (res >= 0) */
	{
	  buff[offset++] = ']';
	  buff[offset++] = '}';
	  buff[offset] = 0;	/* null terminate the string */
	  res = offset;
	}
      break;
    case SEXP:
      /* ('set2 (region) (region) (region)) */
      strcpy(buff, "('" GT_SER_TEXT_Set2);
      offset = strlen(buff);

      for (i = 0; i < Set2_numRegions(input) && (unsigned int)offset < buffSize; ++i)
	{
	  buff[offset++] = ' ';

	  res = Serialize_R(mode, &(Set2_region(input,i)), buff+offset, buffSize-offset);
	  
	  if (res < 0)
	    break; 		/* ERROR, get outta here */

	  offset += res;
	}
      /* if (res >= 0) */
	{
	  buff[offset++] = ')';
	  buff[offset] = 0;	/* null terminate the string */
	  res = offset;
	}
      break;
    case BINARY:
      assert(0); 		/* untested */
      /* byte array (char array)
       [ (8 bit)GT_SER_KEY_S, (unsigned int) numRegions, region, region, region, region,...]*/
      buff[0] = GT_SER_KEY_Set2;
      memcpy(buff+1,&(Set2_numRegions(input)),sizeof(Set2_numRegions(input)));
      offset += 1+sizeof(Set2_numRegions(input));
      res = 0;

      for (i = 0; i < Set2_numRegions(input) && (unsigned int)offset < buffSize; ++i)
	{
	  res = Serialize_R(mode, &(Set2_region(input,i)), buff+offset, buffSize-offset);
	  
	  if (res < 0)
	    break; 		/* ERROR, get outta here */
	  
	  offset += res;
	}
      if (res >= 0)
	  res = offset;

      break;
    default:
      res = -3;
    }
  
  if (res > (int)buffSize) /* you blew through your buffer */
      GT_SER_BUFFER_DEATH;
  
  return (res);
}
int Deserialize_S(const SerializeMode mode, char * input, Set2 * output)
{
  assert(0); 			/* untested */
  return -1;
}

