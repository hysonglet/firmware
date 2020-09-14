# Automatically generated by pb2py
# fmt: off
from .. import protobuf as p

from .ECDSASignature import ECDSASignature

if __debug__:
    try:
        from typing import Dict, List  # noqa: F401
        from typing_extensions import Literal  # noqa: F401
    except ImportError:
        pass


class NodeSignatureReply(p.MessageType):
    MESSAGE_WIRE_TYPE = 835

    def __init__(
        self,
        signature: ECDSASignature = None,
    ) -> None:
        self.signature = signature

    @classmethod
    def get_fields(cls) -> Dict:
        return {
            1: ('signature', ECDSASignature, 0),
        }
