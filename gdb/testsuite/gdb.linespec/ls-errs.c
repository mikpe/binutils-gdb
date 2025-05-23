/* This testcase is part of GDB, the GNU debugger.

   Copyright 2012-2025 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

int
myfunction (int aa)
{
  int i;

  i = aa + 42;

  /* These lines are intentionally left blank such that the tests trying
     to place breakpoints at line -10 relative to the "set.breakpoint.here"
     line below land on a valid breakpoint location, inside the function.  */






  return i;    /* set breakpoint here */
}

int
main (void)
{
  int a;

  a = myfunction (a);

 here:
  return a;
}
