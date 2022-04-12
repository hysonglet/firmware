void fsm_msgSignDigest(const SignDigest *msg) {
  RESP_INIT(DigestSignature);

  CHECK_INITIALIZED

  layoutSignMessage(msg->digest.bytes, msg->digest.size);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  CHECK_PIN

  layoutProgressSwipe(_("Singing"), 0);

  const CoinInfo *coin = fsm_getCoin(msg->has_coin_name, msg->coin_name);
  HDNode *node = fsm_getDerivedNode(coin->curve_name, msg->address_n,
                                    msg->address_n_count, NULL);

  hdnode_sign_digest(node, msg->digest.bytes, resp->signature.bytes, NULL, NULL);
  resp->signature.size = 64;
  resp->has_signature = true;

  ecdsa_get_public_key65(node->curve->params, node->private_key, resp->pubkey.bytes);
  resp->pubkey.size = 65;

  msg_write(MessageType_MessageType_DigestSignature, resp);

  layoutHome();
}


void fsm_msgSignData(const SignData *msg) {
  RESP_INIT(DataSignature);

  CHECK_INITIALIZED

  sha256_Raw(msg->data.bytes, msg->data.size, resp->digest.bytes);
  resp->digest.size = 32;
  resp->has_digest = true;

  layoutSignMessage(resp->digest.bytes, resp->digest.size);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  CHECK_PIN

  layoutProgressSwipe(_("Singing"), 0);

  const CoinInfo *coin = fsm_getCoin(msg->has_coin_name, msg->coin_name);
  HDNode *node = fsm_getDerivedNode(coin->curve_name, msg->address_n,
                                    msg->address_n_count, NULL);

  hdnode_sign_digest(node, resp->digest.bytes, resp->signature.bytes, NULL, NULL);
  resp->signature.size = 64;
  resp->has_signature = true;

  ecdsa_get_public_key65(node->curve->params, node->private_key, resp->pubkey.bytes);
  resp->pubkey.size = 65;

  msg_write(MessageType_MessageType_DataSignature, resp);

  layoutHome();
}

void fsm_msgExportEd25519PublicKey(const ExportEd25519PublicKey *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(Ed25519PublicKey);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);

  if (!node) return;

  hdnode_fill_public_key(node);

  resp->pubkey.size = 32;

  memcpy(&resp->pubkey.bytes, &node->public_key[1],
         sizeof(resp->pubkey.bytes));

  resp->privkey.size = 32;
  memcpy(&resp->privkey.bytes, &node->private_key[0],
         sizeof(resp->privkey.bytes));

  msg_write(MessageType_MessageType_Ed25519PublicKey, resp);

  layoutHome();
}
