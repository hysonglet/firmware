# Automatically generated by pb2py
# fmt: off
from .. import protobuf as p

from .NodeId import NodeId

if __debug__:
    try:
        from typing import Dict, List  # noqa: F401
        from typing_extensions import Literal  # noqa: F401
    except ImportError:
        pass


class GetExtPubKeyRequest(p.MessageType):
    MESSAGE_WIRE_TYPE = 809

    def __init__(
        self,
        node_id: NodeId = None,
    ) -> None:
        self.node_id = node_id

    @classmethod
    def get_fields(cls) -> Dict:
        return {
            1: ('node_id', NodeId, 0),
        }
