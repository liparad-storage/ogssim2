#!/usr/bin/env python

import matplotlib
matplotlib.use ('Agg')

import matplotlib.pyplot as plt
import matplotlib.colors as mcl
import numpy as np
from scipy.interpolate import spline

#colors = ['b', 'g', 'r']

def createLineGraph (filename, values, legendLabels, xLabel, yLabel, ext):

	fig, ax = plt.subplots ()

	if len (values [0]) <= 20:
		xnew = np.linspace (values [0][0], values [0][-1], 300)

		for i in range (len (values) - 1):
			ynew = spline (values [0], values [1 + i], xnew)
			ax.plot (xnew, ynew, label=legendLabels [i])

	else:
		for i in range (len (values) - 1):
			ax.plot (values [0], values [1 + i], label=legendLabels [i])

	ax.set_xlabel (xLabel)
	ax.set_ylabel (yLabel)

	ax.set_ylim (.0);
	plt.tight_layout (w_pad = 0.2)

	ax.legend ()

	plt.savefig (filename, format=ext)

#	plt.show ()
	plt.gcf () .clear ()

def createLineGraphWithRules (filename, values, legendLabels, xLabel, yLabel, rules, ext):
	prop_cycle = plt.rcParams['axes.prop_cycle']
	colors = prop_cycle.by_key()['color']

	fig, (ax1,ax2) = plt.subplots (1,2,sharey=True, facecolor='w')
	
	if len (values [0]) <= 20:
		xnew = np.linspace (values [0][0], values [0][-1], 300)

		for i in range (len (values) - 1):
			ynew = spline (values [0], values [1 + i], xnew)
			ax1.plot (xnew, ynew, label=legendLabels [i])
			ax2.plot (xnew, ynew, label=legendLabels [i])

	else:
		for i in range (len (values) - 1):
			ax1.plot (values [0], values [1 + i], label=legendLabels [i])
			ax2.plot (values [0], values [1 + i], label=legendLabels [i])

	ax1.set_xlabel (xLabel)
	ax1.set_ylabel (yLabel)

	ax1.set_ylim (.0)

	ax1.set_xlim (0,25)
	ax2.set_xlim (50, 3600)

	ax1.spines ['right'] .set_visible (False)
	ax2.spines ['left'] .set_visible (False)
	ax1.yaxis.tick_left()
	ax1.tick_params (labelright='off')

	for i in range (len (rules)):
		ax1.axvline (x=rules[i][1], linestyle='--', label=rules[i][0], color=colors[i+1])
		ax2.axvline (x=rules[i][1], linestyle='--', label=rules[i][0], color=colors[i+1])

	ax1.legend ()

	d = .02
	kwargs = dict (transform=ax1.transAxes, color='k', clip_on=False)
	ax1.plot ((1-d,1+d), (-d,+d), **kwargs)
	ax1.plot ((1-d,1+d), (1-d,1+d), **kwargs)

	kwargs.update (transform=ax2.transAxes)
	ax2.plot((-d,+d), (1-d,1+d), **kwargs)
	ax2.plot((-d,+d), (-d,+d), **kwargs)

	plt.savefig (filename, format=ext)

	plt.gcf () .clear ()

def createLineDoubleAxisGraphWithRules (filename, values, values2, legendLabels, legendLabels2, xLabel, yLabel, yLabel2, rules, maxVal, ext):
	prop_cycle = plt.rcParams['axes.prop_cycle']
	colors = prop_cycle.by_key()['color']

	fig, (ax1,ax2) = plt.subplots (1,2,sharey=True, facecolor='w')
	
	par1 = ax1.twinx ()
	par2 = ax2.twinx ()

	if len (values [0]) <= 20:
		xnew = np.linspace (values [0][0], values [0][-1], 300)

		for i in range (len (values) - 1):
			ynew = spline (values [0], values [1 + i], xnew)
			ax1.plot (xnew, ynew, label='user req.')
			ax2.plot (xnew, ynew, label='user req.')

	else:
		for i in range (len (values) - 1):
			ax1.plot (values [0], values [1 + i], label='user req.')
			ax2.plot (values [0], values [1 + i], label='user req.')

	if len (values2 [0]) <= 20:
		xnew = np.linspace (values2 [0][0], values2 [0][-1], 300)

		for i in range (len (values2) - 1):
			ynew = spline (values2 [0], values2 [1 + i], xnew)
			par1.plot (xnew, ynew, color=colors[3], label='syst req.')
			par2.plot (xnew, ynew, color=colors[3], label='syst req.')

	else:
		for i in range (len (values2) - 1):
			par1.plot (values2 [0], values2 [1 + i], color=colors[2], label='syst req.')
			par2.plot (values2 [0], values2 [1 + i], color=colors[2], label='syst req.')

	fig.text (0.5,.01, xLabel, ha='center', va='center')
	ax1.set_ylabel (yLabel)

	par2.set_ylabel (yLabel2)

	ax1.set_ylim (.0)
	par2.set_ylim (.0)

#	ax1.set_xlim (0,30)
#	ax2.set_xlim (35, ((maxVal / 500) + 1) * 500)

	ax1.set_xlim (0,300)
	ax2.set_xlim (350, ((maxVal / 5000) + 1) * 5000)

	par1.tick_params (axis='y', which='both', left=False, right=False, labelright=False)
	ax2.tick_params (axis='y', which='both', left=False, right=False)

	ax1.spines ['right'] .set_visible (False)
	par1.spines ['right'] .set_visible (False)
	ax2.spines ['left'] .set_visible (False)
	par2.spines ['left'] .set_visible (False)

	ax1.yaxis.tick_left()
	ax1.tick_params (labelright='off')

	ax1.axvline (x=rules[0][1], linestyle='--', label=rules[0][0], color=colors[3])
	ax1.axvline (x=rules[1][1], linestyle='--', label=rules[1][0], color=colors[1])
	ax2.axvline (x=rules[1][1], linestyle='--', label=rules[1][0], color=colors[1])
	print (str (maxVal))
	par2.axvline (x=maxVal, linestyle='--', label=rules[2][0], color=colors[2])

	ax1.legend (prop={'size': 8})
	par2.legend (prop={'size': 8})

	d = .02
	kwargs = dict (transform=ax1.transAxes, color='k', clip_on=False)
	ax1.plot ((1-d/2,1+d/2), (-d/2,+d/2), **kwargs)
	ax1.plot ((1-d/2,1+d/2), (1-d/2,1+d/2), **kwargs)

	kwargs.update (transform=ax2.transAxes)
	ax2.plot((-d/2,+d/2), (1-d/2,1+d/2), **kwargs)
	ax2.plot((-d/2,+d/2), (-d/2,+d/2), **kwargs)

	plt.tight_layout (w_pad = 0.2)

	plt.savefig (filename, format=ext)

	plt.gcf () .clear ()

def createPointGraph (filename, values, legendLabels, xLabel, yLabel, ext):
	fig, ax = plt.subplots ()

	for i in range (len (values) - 1):
		ax.scatter (values [0], values [1 + i], label=legendLabels [i], color=colors [i])

	ax.set_xlabel (xLabel)
	ax.set_ylabel (yLabel)

	ax.legend ()

	plt.savefig (filename, format=ext)
	#plt.show ()
	plt.gcf () .clear ()

def createHistogram (filename, values, legendLabels, xLabel, yLabel, ext):
	prop_cycle = plt.rcParams['axes.prop_cycle']
	colors = prop_cycle.by_key()['color']

	fig, ax = plt.subplots ()

	size = 1. / (len (values) - 1)

	for i in range (len (values) - 1):
		tmp = [i * size] * len (values [0])
		ax.bar (np.array(values [0]) + np.array(tmp), values [1 + i], width = size, label=legendLabels [i], align='center', color=colors [i])

	ax.set_xlabel (xLabel)
	ax.set_ylabel (yLabel)

	ax.legend ()

	plt.savefig (filename, format=ext)
	#plt.show ()
	plt.gcf () .clear ()

def createStackedHistogram (filename, values, legendLabels, xLabel, yLabel, ext):
	prop_cycle = plt.rcParams['axes.prop_cycle']
	colors = prop_cycle.by_key()['color']

	tmp = colors [2]
	colors [2] = colors [1]
	colors [1] = tmp

	p = []

	maxv = 0
	for j in range (len (values [0])):
		tmp = 0
		for i in range (len (values) - 1):
			tmp = tmp + values [1 + i][j]
		if maxv < tmp:
			maxv = tmp

	maxv = maxv * 120 / 100

	bottoms = np.zeros (len (values [0]) )
	for i in range (len (values) - 1):
		p.append (plt.bar (values [0], values [i + 1], bottom=bottoms, align='center', color=colors [i]) )
		for j in range (len (bottoms) ):
			bottoms [j] = bottoms [j] + values [i + 1][j]


	plt.xlabel (xLabel)
	plt.ylabel (yLabel)
	plt.ylim (0, maxv)
	
	ind = np.arange(len (values [0]));
	plt.xticks(ind)

	plt.legend (p, legendLabels)

	plt.savefig (filename, format=ext)
	#plt.show ()
	plt.gcf () .clear ()

def createPieChart (filename, values, legendLabels, ext):
	explode = (0, .1)

	fig, ax = plt.subplots ()
	ax.pie (values, explode=explode, labels=legendLabels, autopct='%1.2f%%', shadow=True, startangle=90)
	ax.axis ('equal')
	plt.savefig (filename, format=ext)
	plt.gcf () .clear ()