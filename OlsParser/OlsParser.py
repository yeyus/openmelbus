import re
import math

class OlsParser:
  # raw lines
  lines = []
  # metadata info
  metadata = []
  sampleRate = 0
  size = 0
  # samples
  samples = []
  # current sample pointer
  currentSample = 0

  class Metadata:
    name = None
    value = None
    def __init__(self, name, value):
      self.name = name
      self.value = value

  class Sample:
    value = None
    sample = None
    ns = None
    def __init__(self, value, sample, ns):
      self.value = value
      self.sample = sample
      self.ns = ns
    def channel(self, number):
      return (int(self.value, 16) & 1 << number) > 0

  # Init function receives path to file
  def __init__(self, file):
    with open(file) as f:
      self.lines = f.readlines()
    self._parseMetadata(self.lines)
    self._parseSamples(self.lines)

  def _parseMetadata(self, lines):
    rawMetadata = []
    for statement in self.lines:
      if statement[0] == ';':
        rawMetadata.append(statement)
    for s in rawMetadata:
      m = re.match(r'\;(?P<key>\w+)\:[ ]*(?P<value>\w+)', s)
      key = m.groupdict()['key']
      value = m.groupdict()['value']
      obj = self.Metadata(key, value)
      self.metadata.append(obj)
      if key == 'Rate':
        self.sampleRate = int(value)
      if key == 'Size':
        self.size = int(value)

  def _parseSamples(self, lines):
    rawSamples = []
    for statement in self.lines:
      if statement[0] is not ';':
        rawSamples.append(statement)
    for s in rawSamples:
      m = re.match(r'(?P<value>\w+)\@(?P<sample>\w+)', s)
      value = m.groupdict()['value']
      sample = int(m.groupdict()['sample'])
      ns = int(math.floor((sample * 1000000000) / self.sampleRate))
      obj = self.Sample(value, sample, ns)
      self.samples.append(obj)

  # Iterator
  def hasNext(self):
    return (self.size - 1) >= self.currentSample

  def next(self):
    obj =self.samples[self.currentSample]
    self.currentSample += 1
    return obj

  def reset(self):
    self.currentSample = 0
