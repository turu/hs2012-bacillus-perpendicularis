hs2012-bacillus-perpendicularis
===============================

Repository of the HS2012 contest problem. Problem code: HS12BAPE

Problem statement:


A group of scientists working for the World Health Organisation have found a new, curious pathogen. Bacillus Perpendicularis (that's what they've called it) is an expansive, rod-shaped bacterium which lives in vast, structured colonies on flat sufaces. Because it can potentially be dangerous to humans, the research group set out to examine it carefully – to find its strengths and weaknesses.

They quickly discovered that a colony always expands in the same curious way and devised a theoretical model of this process. The colony starts with just one pathogen (stage one). In each subsequent stage, for every exposed end, a new perpendicular pathogen appears, centered at that end. The following pictures illustrate the first 4 stages of the process:
[pictures]


The other interesting finding was that once a single pathogen from the colony hits a bacteriostat (a biological or chemical agent that stops bacteria from reproducing), not only the affected pathogen ceases to reproduce, but the entire colony stops expanding as well. They now want to study this fenomenum more closely. That's why they thought of a test that will help them understand the inner workings of a Bacillus Perpendicularis colony. The test consists of a number of steps:

Prepare a flat, clean surface.

Take a bacteriostat and place it somewhere on the surface.

Place a single bacterium in the middle of the surface (it will shortly start to reproduce, giving birth to a colony).

Watch the colony grow.

When it stops growing, write down the age of the colony.

 

You have been asked to prepare a computer simulation of the above procedure based on the theoretical model desribed earlier, so it can be validated against the observed real world numbers. The simulation assumes that a test surface can be modelled by a square grid of cells and each pathogen can be modelled by a line (2 units long), either horizontal or vertical.

The starting bacterium always has its middle point placed at (0, 0) and is oriented vertically.

Input

The first line contains a single integer t < 1000 – the number of test cases.

The next t lines contain a description of the test cases. The i-th line of the description consists of two integers x (-10^6 <= x <= 10^6) and y (-10^6 <= y <= 10^6) denoting the coordinates of the bacteriostat.

Output

Output consists of t lines. The i-th line of output contains the number of the stage (we start counting from 1) at which the colony stopped expanding, or -1 if it would expand forever.

Example

  Input:
  2
  2 2
  2 0
  Output:
  4
  -1

Scoring

By solving this problem you score 10 points.
