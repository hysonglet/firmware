from trezor.lvglui.scrs.components.button import NormalButton
from trezor.lvglui.scrs.components.pageable import PageAbleMessage

from ..i18n import gettext as _, keys as i18n_keys
from ..lv_colors import lv_colors
from . import font_PJSBOLD36
from .common import FullSizeWindow, lv
from .components.container import ContainerFlexCol
from .components.listitem import DisplayItem
from .components.qrcode import QRCode


class Address(FullSizeWindow):
    def __init__(self, title, path, address, xpubs=None, multisig_index=0):
        super().__init__(
            title, None, confirm_text=_(i18n_keys.BUTTON__DONE), anim_dir=2
        )
        self.qr = QRCode(self.content_area, address)
        self.qr.align_to(self.title, lv.ALIGN.OUT_BOTTOM_MID, 0, 48)
        self.container = ContainerFlexCol(self.content_area, self.qr)
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__PATH__COLON), path
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__ADDRESS__COLON), address
        )
        if xpubs:
            self.title.align(lv.ALIGN.TOP_MID, 0, 100)
            self.btn_yes.align(lv.ALIGN.BOTTOM_MID, 0, -30)
        for i, xpub in enumerate(xpubs or []):
            self.item3 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__XPUB_STR_MINE__COLON).format(i + 1)
                if i == multisig_index
                else _(i18n_keys.LIST_KEY__XPUB_STR_COSIGNER__COLON).format(i + 1),
                xpub,
            )


class XpubOrPub(FullSizeWindow):
    def __init__(self, title, path, xpub=None, pubkey=None):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__EXPORT),
            _(i18n_keys.BUTTON__CANCEL),
            anim_dir=2,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__PATH__COLON), path
        )
        self.item2 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__XPUB__COLON)
            if xpub
            else _(i18n_keys.LIST_KEY__PUBLIC_KEY__COLON),
            xpub or pubkey,
        )


class Message(FullSizeWindow):
    def __init__(self, title, address, message):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__SIGN),
            _(i18n_keys.BUTTON__CANCEL),
            anim_dir=2,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__ADDRESS__COLON), address
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MESSAGE__COLON), message
        )
        if len(message) > 100:
            self.show_full_message = NormalButton(
                self, _(i18n_keys.BUTTON__VIEW_FULL_MESSAGE)
            )
            self.show_full_message.align_to(self.item2, lv.ALIGN.OUT_BOTTOM_MID, 0, 32)
            self.show_full_message.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
            self.message = message

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.show_full_message:
                PageAbleMessage(self.message, _(i18n_keys.BUTTON__CLOSE))


class TransactionOverview(FullSizeWindow):
    def __init__(self, title, amount, address):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
            anim_dir=2,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container,
            f"{_(i18n_keys.INSERT__SEND)} #FFFFFF {amount} # {_(i18n_keys.INSERT__TO)}:",
            address,
        )
        self.item1.label_top.set_recolor(True)


class TransactionDetailsETH(FullSizeWindow):
    def __init__(
        self,
        title,
        address_from,
        address_to,
        amount,
        fee_max,
        is_eip1559=False,
        gas_price=None,
        max_priority_fee_per_gas=None,
        max_fee_per_gas=None,
        total_amount=None,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__SLIDE_TO_SIGN),
            _(i18n_keys.BUTTON__CANCEL),
            hold_confirm=True,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        if not is_eip1559:
            self.item2 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__GAS_PRICE__COLON), gas_price
            )
        else:
            self.item2 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__PRIORITY_FEE_PER_GAS__COLON),
                max_priority_fee_per_gas,
            )
            self.item2_1 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__MAXIMUM_FEE_PER_GAS__COLON),
                max_fee_per_gas,
            )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee_max
        )
        self.item4 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), address_to
        )
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), address_from
        )
        if total_amount is None:
            total_amount = f"{amount}\n{fee_max}"
        self.item6 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TOTAL_AMOUNT__COLON), total_amount
        )


class ContractDataOverview(FullSizeWindow):
    def __init__(self, title, description, data):
        super().__init__(
            title, None, _(i18n_keys.BUTTON__CONTINUE), _(i18n_keys.BUTTON__CANCEL)
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__SIZE__COLON), description
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__DATA__COLON), data
        )
        if len(data) > 100:
            self.show_full_data = NormalButton(
                self, _(i18n_keys.BUTTON__VIEW_FULL_DATA)
            )
            self.show_full_data.align_to(self.item2, lv.ALIGN.OUT_BOTTOM_MID, 0, 32)
            self.show_full_data.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
            self.data = data

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.show_full_data:
                PageAbleMessage(self.data, _(i18n_keys.BUTTON__CLOSE))


class BlobDisPlay(FullSizeWindow):
    def __init__(
        self,
        title,
        description: str,
        content: str,
        icon_path: str = "A:/res/warning.png",
        anim_dir: int = 1,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
            icon_path=icon_path,
            anim_dir=anim_dir,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(self.container, description, content)
        if len(content) > 300:
            self.show_full_data = NormalButton(
                self, _(i18n_keys.BUTTON__VIEW_FULL_DATA)
            )
            self.show_full_data.align_to(self.item1, lv.ALIGN.OUT_BOTTOM_MID, 0, 32)
            self.show_full_data.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
            self.data = content

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.show_full_data:
                PageAbleMessage(self.data, _(i18n_keys.BUTTON__CLOSE))


class ConfirmMetaData(FullSizeWindow):
    def __init__(self, title, subtitle, description, data):
        super().__init__(
            title, subtitle, _(i18n_keys.BUTTON__CONTINUE), _(i18n_keys.BUTTON__CANCEL)
        )
        if description:
            self.container = ContainerFlexCol(
                self.content_area, self.subtitle, pos=(0, 48)
            )
            self.item1 = DisplayItem(self.container, description, data)


class TransactionDetailsBTC(FullSizeWindow):
    def __init__(self, title: str, amount: str, fee: str, total: str):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__SLIDE_TO_SIGN),
            _(i18n_keys.BUTTON__CANCEL),
            hold_confirm=True,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(self.container, _(i18n_keys.LIST_KEY__FEE__COLON), fee)
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TOTAL_AMOUNT__COLON), total
        )


class JointTransactionDetailsBTC(FullSizeWindow):
    def __init__(self, title: str, amount: str, total: str):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__SLIDE_TO_SIGN),
            _(i18n_keys.BUTTON__CANCEL),
            hold_confirm=True,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT_YOU_SPEND__COLON), amount
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TOTAL_AMOUNT__COLON), total
        )


class ModifyFee(FullSizeWindow):
    def __init__(self, description: str, fee_change: str, fee_new: str):
        super().__init__(
            _(i18n_keys.TITLE__MODIFY_FEE),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(self.container, description, fee_change)
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__NEW_FEE__COLON), fee_new
        )


class ModifyOutput(FullSizeWindow):
    def __init__(
        self, address: str, description: str, amount_change: str, amount_new: str
    ):
        super().__init__(
            _(i18n_keys.TITLE__MODIFY_AMOUNT),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__ADDRESS__COLON), address
        )
        self.item2 = DisplayItem(self.container, description, amount_change)
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__NEW_AMOUNT__COLON), amount_new
        )


class ConfirmReplacement(FullSizeWindow):
    def __init__(self, title: str, txid: str):
        super().__init__(
            title, None, _(i18n_keys.BUTTON__CONTINUE), _(i18n_keys.BUTTON__CANCEL)
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TRANSACTION_ID__COLON), txid
        )


class ConfirmPaymentRequest(FullSizeWindow):
    def __init__(self, title: str, subtitle, amount: str, to_addr: str):
        super().__init__(
            title,
            subtitle,
            _(i18n_keys.BUTTON__SLIDE_TO_SIGN),
            _(i18n_keys.BUTTON__CANCEL),
            hold_confirm=True,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), to_addr
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )


class ConfirmDecredSstxSubmission(FullSizeWindow):
    def __init__(self, title: str, subtitle: str, amount: str, to_addr: str):
        super().__init__(
            title, subtitle, _(i18n_keys.BUTTON__CONTINUE), _(i18n_keys.BUTTON__CANCEL)
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, -(i18n_keys.LIST_KEY__TO__COLON), to_addr
        )


class ConfirmCoinJoin(FullSizeWindow):
    def __init__(
        self, title: str, coin_name: str, max_rounds: str, max_fee_per_vbyte: str
    ):
        super().__init__(
            title, None, _(i18n_keys.BUTTON__CONTINUE), _(i18n_keys.BUTTON__CANCEL)
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__COIN_NAME__COLON), coin_name
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_ROUNDS__COLON), max_rounds
        )
        self.item3 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__MAXIMUM_MINING_FEE__COLON),
            max_fee_per_vbyte,
        )


class ConfirmSignIdentity(FullSizeWindow):
    def __init__(self, title: str, identity: str, subtitle: str | None):
        super().__init__(
            title, subtitle, _(i18n_keys.BUTTON__CONTINUE), _(i18n_keys.BUTTON__CANCEL)
        )
        align_base = self.title if subtitle is None else self.subtitle
        self.container = ContainerFlexCol(self.content_area, align_base, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__IDENTITY__COLON), identity
        )


class ConfirmProperties(FullSizeWindow):
    def __init__(self, title: str, properties: list[tuple[str, str]]):
        super().__init__(
            title, None, _(i18n_keys.BUTTON__CONTINUE), _(i18n_keys.BUTTON__CANCEL)
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        for key, value in properties:
            self.item = DisplayItem(self.container, f"{key.upper()}", value)


class ConfirmTransferBinance(FullSizeWindow):
    def __init__(self, items: list[tuple[str, str, str]]):
        super().__init__(
            _(i18n_keys.TITLE__CONFIRM_TRANSFER),
            None,
            _(i18n_keys.BUTTON__SLIDE_TO_SIGN),
            _(i18n_keys.BUTTON__CANCEL),
            hold_confirm=True,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        for key, value, address in items:
            self.item1 = DisplayItem(self.container, key, "")
            self.item2 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), value
            )
            self.item3 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__TO__COLON), address
            )


class ShouldShowMore(FullSizeWindow):
    def __init__(self, title: str, key: str, value: str, button_text: str):
        super().__init__(
            title, None, _(i18n_keys.BUTTON__CONTINUE), _(i18n_keys.BUTTON__CANCEL)
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item = DisplayItem(self.container, f"{key}:", value)
        self.show_more = NormalButton(self.content_area, button_text)
        self.show_more.align_to(self.container, lv.ALIGN.OUT_BOTTOM_MID, 0, 32)
        self.show_more.add_event_cb(self.on_show_more, lv.EVENT.CLICKED, None)

    def on_show_more(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.show_more:
                # 2 means show more
                self.channel.publish(2)
                self.destroy()


class EIP712DOMAIN(FullSizeWindow):
    def __init__(self, title: str, **kwargs):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
            anim_dir=2,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        if kwargs.get("name"):
            self.item1 = DisplayItem(
                self.container, "name (string):", kwargs.get("name")
            )
        if kwargs.get("version"):
            self.item2 = DisplayItem(
                self.container, "version (string):", kwargs.get("version")
            )
        if kwargs.get("chainId"):
            self.item3 = DisplayItem(
                self.container, "chainId (uint256):", kwargs.get("chainId")
            )
        if kwargs.get("verifyingContract"):
            self.item4 = DisplayItem(
                self.container,
                "verifyingContract (address):",
                kwargs.get("verifyingContract"),
            )
        if kwargs.get("salt"):
            self.item5 = DisplayItem(
                self.container, "salt (bytes32):", kwargs.get("salt")
            )


class TransactionDetailsTRON(FullSizeWindow):
    def __init__(
        self,
        title,
        address_from,
        address_to,
        amount,
        fee_max,
        total_amount=None,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__SLIDE_TO_SIGN),
            _(i18n_keys.BUTTON__CANCEL),
            hold_confirm=True,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee_max
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), address_to
        )
        self.item4 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), address_from
        )
        if total_amount is None:
            total_amount = f"{amount}\n{fee_max}"
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TOTAL_AMOUNT__COLON), total_amount
        )


class SecurityCheck(FullSizeWindow):
    def __init__(self):
        super().__init__(
            title=_(i18n_keys.TITLE__SECURITY_CHECK),
            subtitle=_(i18n_keys.SUBTITLE__SECURITY_CHECK),
            confirm_text=_(i18n_keys.BUTTON__CONFIRM),
            cancel_text=_(i18n_keys.BUTTON__CANCEL),
            icon_path="A:/res/security-check.png",
            anim_dir=2,
        )


class PassphraseDisplayConfirm(FullSizeWindow):
    def __init__(self, passphrase: str):
        super().__init__(
            title=_(i18n_keys.TITLE__USE_THIS_PASSPHRASE),
            subtitle=_(i18n_keys.SUBTITLE__USE_THIS_PASSPHRASE),
            confirm_text=_(i18n_keys.BUTTON__CONFIRM),
            cancel_text=_(i18n_keys.BUTTON__CANCEL),
            anim_dir=0,
        )

        self.panel = lv.obj(self.content_area)
        self.panel.remove_style_all()
        self.panel.set_size(432, lv.SIZE.CONTENT)
        self.panel.set_style_min_height(80, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.align_to(self.subtitle, lv.ALIGN.OUT_BOTTOM_MID, 0, 48)
        self.panel.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.set_style_bg_color(
            lv_colors.ONEKEY_BLACK_1, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.panel.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.set_style_pad_all(16, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.set_style_radius(16, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.set_style_text_color(
            lv_colors.LIGHT_GRAY, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.panel.set_style_text_font(font_PJSBOLD36, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.content = lv.label(self.panel)
        self.content.set_size(400, lv.SIZE.CONTENT)
        self.content.set_text(passphrase)
        self.content.set_long_mode(lv.label.LONG.WRAP)
        self.content.set_style_text_align(
            lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
        )


class SolBlindingSign(FullSizeWindow):
    def __init__(self, fee_payer: str, message_hex: str):
        super().__init__(
            _(i18n_keys.TITLE__VIEW_TRANSACTION),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__FORMAT__COLON),
            _(i18n_keys.LIST_VALUE__UNKNOWN__COLON),
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MESSAGE_HASH__COLON), message_hex
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FEE_PAYER__COLON), fee_payer
        )


class SolTransfer(FullSizeWindow):
    def __init__(self, from_addr: str, fee_payer: str, to_addr: str, amount: str):
        super().__init__(
            title=_(i18n_keys.TITLE__STR_TRANSFER).format("SOL"),
            subtitle=None,
            confirm_text=_(i18n_keys.BUTTON__CONTINUE),
            cancel_text=_(i18n_keys.BUTTON__CANCEL),
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), to_addr
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), from_addr
        )
        self.item4 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FEE_PAYER__COLON), fee_payer
        )


class SolCreateAssociatedTokenAccount(FullSizeWindow):
    def __init__(
        self,
        fee_payer: str,
        funding_account: str,
        associated_token_account: str,
        wallet_address: str,
        token_mint: str,
    ):
        super().__init__(
            title=_(i18n_keys.TITLE__CREATE_TOKEN_ACCOUNT),
            subtitle=None,
            confirm_text=_(i18n_keys.BUTTON__CONTINUE),
            cancel_text=_(i18n_keys.BUTTON__CANCEL),
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__NEW_TOKEN_ACCOUNT),
            associated_token_account,
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__OWNER), wallet_address
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MINT_ADDRESS), token_mint
        )
        self.item4 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FUNDED_BY__COLON), funding_account
        )
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FEE_PAYER__COLON), fee_payer
        )  # _(i18n_keys.LIST_KEY__FEE_PAYER__COLON)


class SolTokenTransfer(FullSizeWindow):
    def __init__(
        self,
        from_addr: str,
        to: str,
        amount: str,
        source_owner: str,
        fee_payer: str,
        token_mint: str | None = None,
    ):
        super().__init__(
            title=_(i18n_keys.TITLE__TOKEN_TRANSFER),
            subtitle=None,
            confirm_text=_(i18n_keys.BUTTON__CONTINUE),
            cancel_text=_(i18n_keys.BUTTON__CANCEL),
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO_TOKEN_ACCOUNT__COLON), to
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM_TOKEN_ACCOUNT__COLON), from_addr
        )
        self.item4 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__SIGNER__COLON), source_owner
        )
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FEE_PAYER__COLON), fee_payer
        )
        if token_mint:
            self.item6 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__MINT_ADDRESS), token_mint
            )


class BlindingSignCommon(FullSizeWindow):
    def __init__(self, signer: str):
        super().__init__(
            _(i18n_keys.TITLE__VIEW_TRANSACTION),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__FORMAT__COLON),
            _(i18n_keys.LIST_VALUE__UNKNOWN__COLON),
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__SIGNER__COLON), signer
        )


class Modal(FullSizeWindow):
    def __init__(
        self,
        title: str | None,
        subtitle: str | None,
        confirm_text: str = "",
        cancel_text: str = "",
        icon_path: str | None = None,
        anim_dir: int = 1,
    ):
        super().__init__(
            title, subtitle, confirm_text, cancel_text, icon_path, anim_dir=anim_dir
        )

    # def show_unload_anim(self):
    #     return super().show_dismiss_anim()


class AlgoCommon(FullSizeWindow):
    def __init__(self, type: str):
        super().__init__(
            _(i18n_keys.TITLE__VIEW_TRANSACTION),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__FORMAT__COLON),
            type,
        )


class AlgoPayment(FullSizeWindow):
    def __init__(
        self,
        sender,
        receiver,
        close_to,
        rekey_to,
        genesis_id,
        note,
        fee,
        amount,
    ):
        super().__init__(
            _(i18n_keys.TITLE__SIGN_STR_TRANSACTION).format("ALGO"),
            None,
            _(i18n_keys.BUTTON__SLIDE_TO_SIGN),
            _(i18n_keys.BUTTON__CANCEL),
            hold_confirm=True,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), receiver
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        self.item4 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item5 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        if close_to is not None:
            self.item6 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__CLOSE_REMAINDER_TO__COLON),
                close_to,
            )
        if rekey_to is not None:
            self.item7 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )
        if genesis_id is not None:
            self.item8 = DisplayItem(self.container, "GENESIS ID:", genesis_id)


class AlgoAssetFreeze(FullSizeWindow):
    def __init__(
        self,
        sender,
        rekey_to,
        fee,
        index,
        target,
        new_freeze_state,
        genesis_id,
        note,
    ):
        super().__init__(
            _(i18n_keys.TITLE__SIGN_STR_TRANSACTION).format("ALGO"),
            None,
            _(i18n_keys.BUTTON__SLIDE_TO_SIGN),
            _(i18n_keys.BUTTON__CANCEL),
            hold_confirm=True,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))

        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FREEZE_ACCOUNT__COLON), target
        )
        self.item3 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__FREEZE_ASSET_ID__COLON),
            _(i18n_keys.LIST_VALUE__TRUE)
            if new_freeze_state is True
            else _(i18n_keys.LIST_VALUE__FALSE),
        )
        self.item4 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__ASSET_ID__COLON), index
        )
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item6 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        if rekey_to is not None:
            self.item7 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )
        if genesis_id is not None:
            self.item8 = DisplayItem(self.container, "GENESIS ID:", genesis_id)


class AlgoAssetXfer(FullSizeWindow):
    def __init__(
        self,
        sender,
        receiver,
        index,
        fee,
        amount,
        close_assets_to,
        revocation_target,
        rekey_to,
        genesis_id,
        note,
    ):
        super().__init__(
            _(i18n_keys.TITLE__SIGN_STR_TRANSACTION).format("ALGO"),
            None,
            _(i18n_keys.BUTTON__SLIDE_TO_SIGN),
            _(i18n_keys.BUTTON__CANCEL),
            hold_confirm=True,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), receiver
        )
        if revocation_target is not None:
            self.item4 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__REVOCATION_ADDRESS__COLON),
                revocation_target,
            )
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__ASSET_ID__COLON), index
        )
        self.item6 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item7 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        if rekey_to is not None:
            self.item8 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )
        if close_assets_to is not None:
            self.item9 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__CLOSE_ASSET_TO__COLON),
                close_assets_to,
            )
        if genesis_id is not None:
            self.item10 = DisplayItem(self.container, "GENESIS ID:", genesis_id)


class AlgoAssetCfg(FullSizeWindow):
    def __init__(
        self,
        fee,
        sender,
        index,
        total,
        default_frozen,
        unit_name,
        asset_name,
        decimals,
        manager,
        reserve,
        freeze,
        clawback,
        url,
        metadata_hash,
        rekey_to,
        genesis_id,
        note,
    ):
        super().__init__(
            _(i18n_keys.TITLE__SIGN_STR_TRANSACTION).format("ALGO"),
            None,
            _(i18n_keys.BUTTON__SLIDE_TO_SIGN),
            _(i18n_keys.BUTTON__CANCEL),
            hold_confirm=True,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))

        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        if asset_name is not None:
            self.item2 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__ASSET_NAME__COLON), asset_name
            )
        if index is not None and index != "0":
            self.item3 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__ASSET_ID__COLON), index
            )
        if url is not None:
            self.item14 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__URL__COLON), url
            )
        if manager is not None:
            self.item5 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__MANAGER_ADDRESS__COLON), manager
            )
        if reserve is not None:
            self.item6 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__RESERVE_ADDRESS__COLON), reserve
            )
        if clawback is not None:
            self.item7 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__CLAW_BACK_ADDRESS__COLON),
                clawback,
            )
        if default_frozen is not None:
            self.item8 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__FREEZE_ADDRESS__QUESTION),
                _(i18n_keys.LIST_VALUE__TRUE)
                if default_frozen is True
                else _(i18n_keys.LIST_VALUE__FALSE),
            )
        if freeze is not None:
            self.item9 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__FREEZE_ADDRESS__COLON), freeze
            )
        if total is not None:
            self.item10 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__TOTAL__COLON), total
            )
        if decimals is not None and decimals != "0":
            self.item11 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__DECIMALS__COLON), decimals
            )
        if unit_name is not None:
            self.item12 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__UNIT_NAME__COLON), unit_name
            )
        if metadata_hash is not None:
            self.item13 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__METADATA_HASH__COLON),
                metadata_hash,
            )
        self.item14 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item15 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        if rekey_to is not None:
            self.item16 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )
        if genesis_id is not None:
            self.item17 = DisplayItem(self.container, "GENESIS ID:", genesis_id)


class AlgoKeyregOnline(FullSizeWindow):
    def __init__(
        self,
        sender,
        fee,
        votekey,
        selkey,
        sprfkey,
        rekey_to,
        genesis_id,
        note,
    ):
        super().__init__(
            _(i18n_keys.TITLE__SIGN_STR_TRANSACTION).format("ALGO"),
            None,
            _(i18n_keys.BUTTON__SLIDE_TO_SIGN),
            _(i18n_keys.BUTTON__CANCEL),
            hold_confirm=True,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))

        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__VRF_PUBLIC_KEY__COLON), selkey
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__VOTE_PUBLIC_KEY__COLON), votekey
        )
        if sprfkey is not None:
            self.item4 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__STATE_PROOF_PUBLIC_KEY__COLON),
                sprfkey,
            )
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item6 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        if rekey_to is not None:
            self.item7 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )
        if genesis_id is not None:
            self.item8 = DisplayItem(self.container, "GENESIS ID:", genesis_id)


class AlgoKeyregNonp(FullSizeWindow):
    def __init__(self, sender, fee, nonpart, rekey_to, genesis_id, note):
        super().__init__(
            _(i18n_keys.TITLE__SIGN_STR_TRANSACTION).format("ALGO"),
            None,
            _(i18n_keys.BUTTON__SLIDE_TO_SIGN),
            _(i18n_keys.BUTTON__CANCEL),
            hold_confirm=True,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))

        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item3 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        self.item4 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__NONPARTICIPATION__COLON),
            _(i18n_keys.LIST_VALUE__FALSE)
            if nonpart is True
            else _(i18n_keys.LIST_VALUE__TRUE),
        )
        if rekey_to is not None:
            self.item5 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )


class AlgoApplication(FullSizeWindow):
    def __init__(self, signer: str):
        super().__init__(
            _(i18n_keys.TITLE__VIEW_TRANSACTION),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__FORMAT__COLON),
            "Application",
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__SIGNER__COLON), signer
        )
