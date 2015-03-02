#!/bin/sh

amplxe-cl -collect knc-general-exploration -knob enable-vpu-metrics=true -knob enable-tlb-metrics=true -knob target-cards=0 -app-working-dir /home/xblee/ParWiBench/src/Equalizing/mic -app-working-dir /home/xblee/ParWiBench/src/Equalizing --search-dir all:rp=/home/xblee/ParWiBench/src/Equalizing/mic --search-dir all:rp=/home/xblee/ParWiBench/src/Equalizing -- ssh mic0 /home/xblee/ParWiBench/src/Equalizing/mic/CEEQ_all.out 0
