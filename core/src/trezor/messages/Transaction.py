# Automatically generated by pb2py
# fmt: off
import protobuf as p

from .InputDescriptor import InputDescriptor
from .OutputDescriptor import OutputDescriptor

if __debug__:
    try:
        from typing import Dict, List  # noqa: F401
        from typing_extensions import Literal  # noqa: F401
    except ImportError:
        pass


class Transaction(p.MessageType):
    MESSAGE_WIRE_TYPE = 851

    def __init__(
        self,
        raw_tx_bytes: bytes = None,
        input_descs: List[InputDescriptor] = None,
        output_descs: List[OutputDescriptor] = None,
    ) -> None:
        self.raw_tx_bytes = raw_tx_bytes
        self.input_descs = input_descs if input_descs is not None else []
        self.output_descs = output_descs if output_descs is not None else []

    @classmethod
    def get_fields(cls) -> Dict:
        return {
            1: ('raw_tx_bytes', p.BytesType, 0),
            2: ('input_descs', InputDescriptor, p.FLAG_REPEATED),
            3: ('output_descs', OutputDescriptor, p.FLAG_REPEATED),
        }
