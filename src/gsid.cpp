/*
 * GOATTRACKER reSID interface
 */

#define GSID_C

#include <stdlib.h>
#include <stdio.h>
#include "residfp/SID.h"

#include "gsid.h"
#include "gsound.h"

int clockrate;
int samplerate;
unsigned char sidreg[NUMSIDREGS];
unsigned char sidorder[] =
  {0x15,0x16,0x18,0x17,
   0x05,0x06,0x02,0x03,0x00,0x01,0x04,
   0x0c,0x0d,0x09,0x0a,0x07,0x08,0x0b,
   0x13,0x14,0x10,0x11,0x0e,0x0f,0x12};

unsigned char altsidorder[] =
  {0x15,0x16,0x18,0x17,
   0x04,0x00,0x01,0x02,0x03,0x05,0x06,
   0x0b,0x07,0x08,0x09,0x0a,0x0c,0x0d,
   0x12,0x0e,0x0f,0x10,0x11,0x13,0x14};

reSIDfp::SID *sid = nullptr;

extern unsigned residdelay;
extern unsigned adparam;

void sid_init(int speed, unsigned m, unsigned ntsc, unsigned interpolate, unsigned customclockrate, float filterbias, unsigned combwaves)
{
  if (ntsc) clockrate = NTSCCLOCKRATE;
    else clockrate = PALCLOCKRATE;

  if (customclockrate)
    clockrate = customclockrate;

  samplerate = speed;

  if (!sid) sid = new reSIDfp::SID;

  switch(interpolate)
  {
    case 0:
        sid->setSamplingParameters(clockrate, reSIDfp::DECIMATE, speed);
        break;

    default:
    case 1:
        sid->setSamplingParameters(clockrate, reSIDfp::RESAMPLE, speed);
        break;
  }

  sid->reset();
  sid->setFilter6581Curve(filterbias);
  sid->setFilter8580Curve(filterbias);
  switch(combwaves)
  {
    case 0:
        sid->setCombinedWaveforms(reSIDfp::WEAK);
        break;

    default:
    case 1:
        sid->setCombinedWaveforms(reSIDfp::AVERAGE);
        break;

    case 2:
        sid->setCombinedWaveforms(reSIDfp::STRONG);
        break;
  }


  for (int c = 0; c < NUMSIDREGS; c++)
  {
    sidreg[c] = 0x00;
  }
  if (m == 1)
  {
    sid->setChipModel(reSIDfp::MOS8580);
  }
  else
  {
    sid->setChipModel(reSIDfp::MOS6581);
  }
}

unsigned char sid_getorder(unsigned char index)
{
  if (adparam >= 0xf000)
    return altsidorder[index];
  else
    return sidorder[index];
}

int sid_fillbuffer(short *ptr, int samples)
{
  if (!sid)
    return 0;

  int tdelta2;
  int result = 0;
  int total = 0;

  int badline = rand() % NUMSIDREGS;

  int tdelta = clockrate * samples / samplerate;
  if (tdelta <= 0) return total;

  for (int c = 0; c < NUMSIDREGS; c++)
  {
    unsigned char o = sid_getorder(c);

    // Possible random badline delay once per writing
    if ((badline == c) && (residdelay))
    {
      tdelta2 = residdelay;
      result = sid->clock(tdelta2, ptr);
      total += result;
      ptr += result;
      samples -= result;
      tdelta -= residdelay;
    }

    sid->write(o, sidreg[o]);

    tdelta2 = SIDWRITEDELAY;
    result = sid->clock(tdelta2, ptr);
    total += result;
    ptr += result;
    samples -= result;
    tdelta -= SIDWRITEDELAY;

    if (tdelta <= 0) return total;
  }

  result = sid->clock(tdelta, ptr);
  total += result;
  ptr += result;
  samples -= result;

  // Loop extra cycles until all samples produced
  while (samples)
  {
    tdelta = clockrate * samples / samplerate;
    if (tdelta <= 0) return total;

    result = sid->clock(tdelta, ptr);
    total += result;
    ptr += result;
    samples -= result;
  }

  return total;
}
