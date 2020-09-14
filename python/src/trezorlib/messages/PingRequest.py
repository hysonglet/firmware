# Automatically generated by pb2py
# fmt: off
from .. import protobuf as p

if __debug__:
    try:
        from typing import Dict, List  # noqa: F401
        from typing_extensions import Literal  # noqa: F401
    except ImportError:
        pass


class PingRequest(p.MessageType):
    MESSAGE_WIRE_TYPE = 805

    def __init__(
        self,
        message: str = None,
    ) -> None:
        self.message = message

    @classmethod
    def get_fields(cls) -> Dict:
        return {
            1: ('message', p.UnicodeType, 0),
        }
