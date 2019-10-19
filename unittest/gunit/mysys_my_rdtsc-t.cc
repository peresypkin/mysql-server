/* Copyright (c) 2009, 2012, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is also distributed with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have included with MySQL.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA */


/*
  rdtsc3 -- multi-platform timer code
  pgulutzan@mysql.com, 2005-08-29
  modified 2008-11-02

  When you run rdtsc3, it will print the contents of
  "my_timer_info". The display indicates
  what timer routine is best for a given platform.

  For example, this is the display on production.mysql.com,
  a 2.8GHz Xeon with Linux 2.6.17, gcc 3.3.3:

  cycles        nanoseconds   microseconds  milliseconds  ticks
------------- ------------- ------------- ------------- -------------
            1            11            13            18            17
   2815019607    1000000000       1000000          1049           102
            1          1000             1             1             1
           88          4116          3888          4092          2044

  The first line shows routines, e.g. 1 = MY_TIMER_ROUTINE_ASM_X86.
  The second line shows frequencies, e.g. 2815019607 is nearly 2.8GHz.
  The third line shows resolutions, e.g. 1000 = very poor resolution.
  The fourth line shows overheads, e.g. ticks takes 2044 cycles.
*/

// First include (the generated) my_config.h, to get correct platform defines.
#include "my_config.h"
#include <gtest/gtest.h>

#include "my_global.h"
#include "my_rdtsc.h"

namespace mysys_my_rdtsc_unittest {

const int LOOP_COUNT= 100;

class RDTimeStampCounter : public ::testing::Test
{
protected:
  void SetUp()
  {
    test_init();
  }
  void test_init();

  MY_TIMER_INFO myt;
};


void RDTimeStampCounter::test_init()
{
  my_timer_init(&myt);

/*
  diag("----- Routine ---------------");
  diag("myt.cycles.routine          : %13llu", myt.cycles.routine);
  diag("myt.nanoseconds.routine     : %13llu", myt.nanoseconds.routine);
  diag("myt.microseconds.routine    : %13llu", myt.microseconds.routine);
  diag("myt.milliseconds.routine    : %13llu", myt.milliseconds.routine);
  diag("myt.ticks.routine           : %13llu", myt.ticks.routine);

  diag("----- Frequency -------------");
  diag("myt.cycles.frequency        : %13llu", myt.cycles.frequency);
  diag("myt.nanoseconds.frequency   : %13llu", myt.nanoseconds.frequency);
  diag("myt.microseconds.frequency  : %13llu", myt.microseconds.frequency);
  diag("myt.milliseconds.frequency  : %13llu", myt.milliseconds.frequency);
  diag("myt.ticks.frequency         : %13llu", myt.ticks.frequency);

  diag("----- Resolution ------------");
  diag("myt.cycles.resolution       : %13llu", myt.cycles.resolution);
  diag("myt.nanoseconds.resolution  : %13llu", myt.nanoseconds.resolution);
  diag("myt.microseconds.resolution : %13llu", myt.microseconds.resolution);
  diag("myt.milliseconds.resolution : %13llu", myt.milliseconds.resolution);
  diag("myt.ticks.resolution        : %13llu", myt.ticks.resolution);

  diag("----- Overhead --------------");
  diag("myt.cycles.overhead         : %13llu", myt.cycles.overhead);
  diag("myt.nanoseconds.overhead    : %13llu", myt.nanoseconds.overhead);
  diag("myt.microseconds.overhead   : %13llu", myt.microseconds.overhead);
  diag("myt.milliseconds.overhead   : %13llu", myt.milliseconds.overhead);
  diag("myt.ticks.overhead          : %13llu", myt.ticks.overhead);
*/
}


TEST_F(RDTimeStampCounter, TestCycle)
{
  ulonglong t1= my_timer_cycles();
  ulonglong t2;
  int i;
  int backward= 0;
  int nonzero= 0;

  for (i=0 ; i < LOOP_COUNT ; i++)
  {
    t2= my_timer_cycles();
    if (t1 >= t2)
      backward++;
    if (t2 != 0)
      nonzero++;
    t1= t2;
  }

  /* Expect at most 1 backward, the cycle value can overflow */
  EXPECT_TRUE((backward <= 1)) << "The cycle timer is strictly increasing";

  if (myt.cycles.routine != 0)
    EXPECT_TRUE((nonzero != 0)) << "The cycle timer is implemented";
  else
    EXPECT_TRUE((nonzero == 0))
      << "The cycle timer is not implemented and returns 0";
}


TEST_F(RDTimeStampCounter, TestNanosecond)
{
  ulonglong t1= my_timer_nanoseconds();
  ulonglong t2;
  int i;
  int backward= 0;
  int nonzero= 0;

  for (i=0 ; i < LOOP_COUNT ; i++)
  {
    t2= my_timer_nanoseconds();
    if (t1 > t2)
      backward++;
    if (t2 != 0)
      nonzero++;
    t1= t2;
  }

  EXPECT_TRUE((backward == 0)) << "The nanosecond timer is increasing";

  if (myt.nanoseconds.routine != 0)
    EXPECT_TRUE((nonzero != 0)) << "The nanosecond timer is implemented";
  else
    EXPECT_TRUE((nonzero == 0))
      << "The nanosecond timer is not implemented and returns 0";
}


TEST_F(RDTimeStampCounter, TestMicrosecond)
{
  ulonglong t1= my_timer_microseconds();
  ulonglong t2;
  int i;
  int backward= 0;
  int nonzero= 0;

  for (i=0 ; i < LOOP_COUNT ; i++)
  {
    t2= my_timer_microseconds();
    if (t1 > t2)
      backward++;
    if (t2 != 0)
      nonzero++;
    t1= t2;
  }

  EXPECT_TRUE((backward == 0)) << "The microsecond timer is increasing";

  if (myt.microseconds.routine != 0)
    EXPECT_TRUE((nonzero != 0)) << "The microsecond timer is implemented";
  else
    EXPECT_TRUE((nonzero == 0))
      << "The microsecond timer is not implemented and returns 0";
}


TEST_F(RDTimeStampCounter, TestMillisecond)
{
  ulonglong t1= my_timer_milliseconds();
  ulonglong t2;
  int i;
  int backward= 0;
  int nonzero= 0;

  for (i=0 ; i < LOOP_COUNT ; i++)
  {
    t2= my_timer_milliseconds();
    if (t1 > t2)
      backward++;
    if (t2 != 0)
      nonzero++;
    t1= t2;
  }

  EXPECT_TRUE((backward == 0)) << "The millisecond timer is increasing";

  if (myt.milliseconds.routine != 0)
    EXPECT_TRUE((nonzero != 0)) << "The millisecond timer is implemented";
  else
    EXPECT_TRUE((nonzero == 0))
      << "The millisecond timer is not implemented and returns 0";
}


TEST_F(RDTimeStampCounter, TestTick)
{
  ulonglong t1= my_timer_ticks();
  ulonglong t2;
  int i;
  int backward= 0;
  int nonzero= 0;

  for (i=0 ; i < LOOP_COUNT ; i++)
  {
    t2= my_timer_ticks();
    if (t1 > t2)
      backward++;
    if (t2 != 0)
      nonzero++;
    t1= t2;
  }

  EXPECT_TRUE((backward == 0)) << "The tick timer is increasing";

  if (myt.ticks.routine != 0)
    EXPECT_TRUE((nonzero != 0)) << "The tick timer is implemented";
  else
    EXPECT_TRUE((nonzero == 0))
      << "The tick timer is not implemented and returns 0";
}


}
