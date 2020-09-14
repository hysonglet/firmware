# Automatically generated by pb2py
# fmt: off
from .. import protobuf as p

if __debug__:
    try:
        from typing import Dict, List  # noqa: F401
        from typing_extensions import Literal  # noqa: F401
    except ImportError:
        pass


class HTLCInfo(p.MessageType):
    MESSAGE_WIRE_TYPE = 869

    def __init__(
        self,
        value_sat: int = None,
        payment_hash: bytes = None,
        cltv_expiry: int = None,
    ) -> None:
        self.value_sat = value_sat
        self.payment_hash = payment_hash
        self.cltv_expiry = cltv_expiry

    @classmethod
    def get_fields(cls) -> Dict:
        return {
            1: ('value_sat', p.UVarintType, 0),
            2: ('payment_hash', p.BytesType, 0),
            3: ('cltv_expiry', p.UVarintType, 0),
        }
