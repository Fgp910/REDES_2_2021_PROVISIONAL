#!/bin/bash

sum=0

for i in {1..10} do
	python3 testSaturacion.py | wc -l