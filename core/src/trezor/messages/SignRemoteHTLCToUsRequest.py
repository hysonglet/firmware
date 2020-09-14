# Automatically generated by pb2py
# fmt: off
import protobuf as p

from .ChannelNonce import ChannelNonce
from .NodeId import NodeId
from .PubKey import PubKey
from .Transaction import Transaction

if __debug__:
    try:
        from typing import Dict, List  # noqa: F401
        from typing_extensions import Literal  # noqa: F401
    except ImportError:
        pass


class SignRemoteHTLCToUsRequest(p.MessageType):
    MESSAGE_WIRE_TYPE = 830

    def __init__(
        self,
        node_id: NodeId = None,
        channel_nonce: ChannelNonce = None,
        tx: Transaction = None,
        input: int = None,
        remote_per_commit_point: PubKey = None,
    ) -> None:
        self.node_id = node_id
        self.channel_nonce = channel_nonce
        self.tx = tx
        self.input = input
        self.remote_per_commit_point = remote_per_commit_point

    @classmethod
    def get_fields(cls) -> Dict:
        return {
            1: ('node_id', NodeId, 0),
            2: ('channel_nonce', ChannelNonce, 0),
            3: ('tx', Transaction, 0),
            4: ('input', p.UVarintType, 0),
            5: ('remote_per_commit_point', PubKey, 0),
        }
