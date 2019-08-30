#!/usr/bin/python

# Copyright UVSQ - CEA/DAM/DIF (2019)
# Contributors: Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
#               Soraya ZERTAL       --      soraya.zertal@uvsq.fr


# This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General
# Public License as published per the Free Software Foundation; either version 3 of the License, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.

# You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA

# Script used to transform the server load evolution output file in a mp4 graph file.
# It needs two arguments: the input and the output file paths.

import sys
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.animation as manimation

def createThreshold (contents):
	nbServ = int (contents [0] .split (' ') [0] )
	sLimit = int (contents [0] .split (' ') [1] )
	hLimit = int (contents [0] .split (' ') [2] )

	FFMpegWriter = manimation.writers['ffmpeg']
	metadata = dict(title='Metadata node load balancing', artist='OGMDSim')
	writer = FFMpegWriter(fps=8, metadata=metadata)

	prop_cycle = plt.rcParams['axes.prop_cycle']
	colors = prop_cycle.by_key()['color']

	xbase = []
	for i in range (nbServ):
		xbase.append (i)

	xtop = []
	for l in contents [1:]:
		if (l [0] == ' '):
			continue
		xtop.append ([])
		for a in l.split (' '):
			xtop [-1] .append (float (a) )

	fig, ax = plt.subplots()
	r = plt.bar (xbase, xtop [0], width=.8, align='center')

	ax.set_xlabel('metadata nodes')
	ax.set_ylabel('node load (%)')

	plt.plot ([-.5, nbServ - .5], [sLimit, sLimit], linestyle='--', color=colors[1])
	plt.plot ([-.5, nbServ - .5], [hLimit, hLimit], linestyle='--', color=colors[3])

	n = 0
	t = plt.text (-.3, 105, 'frame ' + str (n) )

	plt.xlim(-.5, nbServ - .5)
	plt.ylim(0, 110)

	n = n + 1

	with writer.saving(fig, sys.argv [2], 100):
		for l in xtop:
			a = sum (l) / len (l)
			for j in range (nbServ):
				r [j] .set_height (l [j])
				if l [j] >= hLimit:
					r [j] .set_color (colors [3])
				elif l [j] >= sLimit:
					r [j] .set_color (colors [1])
				else:
					r [j] .set_color (colors [2])
			t.set_text ('frame ' + str (n) )
			n = n + .5

			writer.grab_frame ()

def createAverage (contents):
	nbServ = int (contents [0] .split (' ') [0] )

	FFMpegWriter = manimation.writers['ffmpeg']
	metadata = dict(title='Metadata node load balancing', artist='OGMDSim')
	writer = FFMpegWriter(fps=15, metadata=metadata)

	prop_cycle = plt.rcParams['axes.prop_cycle']
	colors = prop_cycle.by_key()['color']

	xbase = []
	for i in range (nbServ):
		xbase.append (i)

	xtop = []
	for l in contents [1:]:
		if (l [0] == ' '):
			continue
		xtop.append ([])
		for a in l.split (' '):
			xtop [-1] .append (int (a) )

	fig, ax = plt.subplots()
	r = plt.bar (xbase, xtop [0], width=.8, align='center')

	ax.set_xlabel('metadata nodes')
	ax.set_ylabel('node load (%)')

	plt.xlim(-.5, nbServ - .5)
	plt.ylim(0, 110)

	with writer.saving(fig, sys.argv [2], 100):
		for l in xtop:
			a = sum (l) / len (l)
			for j in range (nbServ):
				r [j] .set_height (l [j])
				if l [j] >= (1 + avgRate) * a:
					r [j] .set_color (colors [3])
				elif l [j] <= (1 - avgRate) * a:
					r [j] .set_color (colors [0])
				else:
					r [j] .set_color (colors [2])

			writer.grab_frame ()

avgRate = .1

if len (sys.argv) != 3:
	print ('ERR: usage - ./load-to-vid.py input output')
	exit (1)

fiit = open (sys.argv [1], 'r')

contents = fiit.readlines ()

fiit.close ()

if len (contents [0] .split (' ') ) == 3:
	createThreshold (contents)
else:
	createAverage (contents)
