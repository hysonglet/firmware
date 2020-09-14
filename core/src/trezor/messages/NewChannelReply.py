# Automatically generated by pb2py
# fmt: off
import protobuf as p

from .ChannelNonce import ChannelNonce

if __debug__:
    try:
        from typing import Dict, List  # noqa: F401
        from typing_extensions import Literal  # noqa: F401
    except ImportError:
        pass


class NewChannelReply(p.MessageType):
    MESSAGE_WIRE_TYPE = 812

    def __init__(
        self,
        channel_nonce: ChannelNonce = None,
    ) -> None:
        self.channel_nonce = channel_nonce

    @classmethod
    def get_fields(cls) -> Dict:
        return {
            1: ('channel_nonce', ChannelNonce, 0),
        }
