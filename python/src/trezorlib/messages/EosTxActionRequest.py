# Automatically generated by pb2py
# fmt: off
from .. import protobuf as p

if __debug__:
    try:
        from typing import Dict, List  # noqa: F401
        from typing_extensions import Literal  # noqa: F401
    except ImportError:
        pass


class EosTxActionRequest(p.MessageType):
    MESSAGE_WIRE_TYPE = 603

    def __init__(
        self,
        *,
        data_size: int = None,
    ) -> None:
        self.data_size = data_size

    @classmethod
    def get_fields(cls) -> Dict:
        return {
            1: ('data_size', p.UVarintType, None),
        }