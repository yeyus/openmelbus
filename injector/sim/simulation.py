# Python Script
from sys import argv
from os.path import splitext, basename

import pysimulavr
from SimUtils import sim_utils
from OlsParser import OlsParser

class XPin(pysimulavr.Pin):

  def __init__(self, dev, name, state = None):
    pysimulavr.Pin.__init__(self)
    self.name = name
    if state is not None: self.SetPin(state)
    # hold the connecting net here, it have not be destroyed, if we leave this method
    self.__net = pysimulavr.Net()
    self.__net.Add(self)
    self.__net.Add(dev.GetPin(name))

  def SetInState(self, pin):
    pysimulavr.Pin.SetInState(self, pin)
    # very verbose pin transitions
    #print "%s='%s' (t=%dns)" % (self.name, pin.toChar(), sim.getCurrentTime())

def toHL(res):
  if res:
    return "H"
  else:
    return "L"

if __name__ == "__main__":

  proc, elffile = argv[1].split(":")

  doGDB = True
  sim = sim_utils.SimulavrAdapter()
  sim.dmanSingleDeviceApplication()
  dev = sim.loadDevice(proc, elffile, doGDB)
  if doGDB:
    gdb = sim.enableGdb(dev, 1212)


  # MCLK Pin 2 - PD2
  mclk = XPin(dev, "D2", "H")
  # MBUSY Pin 3 - PD3
  mbusy = XPin(dev, "D3", "H")
  # MDATA Pin 4 - PD4
  mdata = XPin(dev, "D4", "H")
  # MRUN Pin 5 - PD5
  mrun = XPin(dev, "D5", "H")

  # argv2 is ols file path
  parser = OlsParser.OlsParser(argv[2])
  # argv3 is offset
  offset = int(argv[3])

  # VCD Processing
  doVCD = True
  if doVCD:
    print "all registrered trace values:\n ",
    print "\n  ".join(sim.getAllRegisteredTraceValues())
    sigs = ("IRQ.VECTOR1", "IRQ.VECTOR2", "PORTD.D2-Out", "PORTD.D3-Out", "PORTD.D4-Out", "PORTD.D5-Out")
    sim.setVCDDump(splitext(basename(argv[0]))[0] + ".vcd", sigs)
    print "-" * 20

  sim.dmanStart()
  print "simulation start: (t=%dns)" % sim.getCurrentTime()
  while sim.getCurrentTime() < offset:
    sim.doStep()
    #print "step (t=%dns)" % sim.getCurrentTime()

  print "simulation offset: (t=%dns)" % sim.getCurrentTime()

  while parser.hasNext():
    n = parser.next()
    while sim.getCurrentTime() < offset + n.ns:
      sim.doStep()
    mrun.SetPin(toHL(n.channel(0)))
    mclk.SetPin(toHL(n.channel(1)))
    mbusy.SetPin(toHL(n.channel(2)))
    mdata.SetPin(toHL(n.channel(3)))
    print "simulation step: (t=%dns d=%dns)" % (sim.getCurrentTime(),n.ns)

  print "simulation end: (t=%dns)" % sim.getCurrentTime()

  sim.dmanStop()
  del dev

# EOF
