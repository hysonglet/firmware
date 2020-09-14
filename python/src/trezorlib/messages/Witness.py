# Automatically generated by pb2py
# fmt: off
from .. import protobuf as p

from .BitcoinSignature import BitcoinSignature
from .PubKey import PubKey

if __debug__:
    try:
        from typing import Dict, List  # noqa: F401
        from typing_extensions import Literal  # noqa: F401
    except ImportError:
        pass


class Witness(p.MessageType):
    MESSAGE_WIRE_TYPE = 863

    def __init__(
        self,
        signature: BitcoinSignature = None,
        pubkey: PubKey = None,
    ) -> None:
        self.signature = signature
        self.pubkey = pubkey

    @classmethod
    def get_fields(cls) -> Dict:
        return {
            1: ('signature', BitcoinSignature, 0),
            2: ('pubkey', PubKey, 0),
        }
