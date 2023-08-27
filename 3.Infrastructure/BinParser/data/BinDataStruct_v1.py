import struct


class OpenCVFrameData:

    def __init__(self, raw_data: bytes):
        self.frame_size = None  # long
        self.raw_size = None    # long
        self.timestamp = None   # long
        self.width = None       # int
        self.height = None      # int
        self.channels = None    # int
        self.reserved = None    # int
        self.raw = None         # bytes

        self.endian_format = '<'
    
        if raw_data is not None:
            self.parse(raw_data)

    def parse(self, raw_data: bytes):
        # Get the size of the data frame
        self.frame_size = struct.unpack(self.endian_format + 'Q', raw_data[0:8])[0]

        # Get the size of the raw data
        self.raw_size = struct.unpack(self.endian_format + 'Q', raw_data[8:16])[0]

        # Get the timestamp
        self.timestamp = struct.unpack(self.endian_format + 'Q', raw_data[16:24])[0]

        # Get the width
        self.width = struct.unpack(self.endian_format + 'i', raw_data[24:28])[0]

        # Get the height
        self.height = struct.unpack(self.endian_format + 'i', raw_data[28:32])[0]

        # Get the number of channels
        self.channels = struct.unpack(self.endian_format + 'i', raw_data[32:36])[0]

        # Get the reserved int
        self.reserved = struct.unpack(self.endian_format + 'i', raw_data[36:40])[0]

        # Get the raw data
        self.raw = raw_data[40:]


class CSIDataFrameData:

    def __init__(self, raw_data: bytes):
        self.frame_size = None  # long
        self.raw_size = None    # long
        self.timestamp = None   # long
        self.reserved = None    # long
        self.raw = None         # bytes

        self.endian_format = '<'
    
        if raw_data is not None:
            self.parse(raw_data)

    def parse(self, raw_data: bytes):

        # Get the size of the data frame
        self.frame_size = struct.unpack(self.endian_format + 'Q', raw_data[0:8])[0]

        # Get the size of the raw data
        self.raw_size = struct.unpack(self.endian_format + 'Q', raw_data[8:16])[0]

        # Get the timestamp
        self.timestamp = struct.unpack(self.endian_format + 'Q', raw_data[16:24])[0]

        # Get the reserved long
        self.reserved = struct.unpack(self.endian_format + 'Q', raw_data[24:32])[0]

        # Get the raw data, skip a long
        self.raw = raw_data[32: ]
