Getting started with the wallet SDK
########################################

.. toctree::
  :maxdepth: 4

.. constants:

Constants
---------

.. c:macro:: EXTERNAL_CHAIN

  Indicate the external chain.

.. c:macro:: INTERNAL_CHAIN

  Indicate the internal chain.

.. c:macro:: COIN_TYPE_ELA

  Indicate the coin ELA.

.. api:

APIs
---------

IdentityManager
================

.. cpp:class:: IdentityManager

static class, the entrance of SDK.

ImportFromFile
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: static int ImportFromFile(const std::string& filePath, std::shared_ptr<Identity>* identity)

  import from file.

  **Return**
    result of this function, 0 is succeded, < 0 is failed.

  **Parameter**
    :[in] filePath: local path of the file.
    :[out] identity: shared ptr of identity.

.. warning:: this function is not impelemented yet.


ExportToFile
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: static void ExportToFile(const std::shared_ptr<Identity>& identity, const std::string& filePath)

  Export identity info to file.

  **Parameter**
    :[in] identity: shared ptr of identity.
    :[in] filePath: ocal path of the file.

.. warning:: this function is not impelemented yet.


GetMnemonic
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: static std::string GetMnemonic(const std::string& language, const std::string& words)

  generate mnemonic.

  **Return**
    mnemonic string, 12 words seperated by white space.

  **Parameter**
    :[in] language: mnemonic language, for example english/chinese.
    :[in] words: the words resource, seperated by \n, if the language is english, words should be empty string.


GetSeed
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: static std::string GetSeed(const std::string& mnemonic, const std::string& language, const std::string& words, const std::string& mnemonicPassword)

  Get seed from mnemonic.

  **Return**
    the seed if succeeded, or empty string if failed.

  **Parameter**
    :[in] mnemonic: 12 words, seperated by white space.
    :[in] language: mnemonic language, for example english/chinese.
    :[in] words: the words resource, seperated by \n, if the language is english, words should be empty string.
    :[in] mnemonicPassword: password of mnemonic, will get different seed if not empty.


CreateIdentity
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: static int CreateIdentity(const std::string& localPath, std::shared_ptr<Identity>* identity)

  Create identity object.

  **Return**
    result of this function, 0 is succeded, < 0 is failed.

  **Parameter**
    :[in] localPath: local path to save data.
    :[out] identity: shared ptr of identity.


Identity
================

.. cpp:class:: Identity

  Create wallet, DidManager. Identity is used to manage wallet and did from the same seed.

CreateSingleAddressWallet
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int CreateSingleAddressWallet(const std::string& seed, const std::shared_ptr<BlockChainNode>& node, std::shared_ptr<HDWallet>* wallet)

  Create single address wallet.

  **Return**
    result of this function, 0 is succeded, < 0 is failed.

  **Parameter**
    :[in] seed: the seed.
    :[in] node: the block chain node.
    :[out] wallet: shared ptr of HDWallet if succeeded, nullptr if failed.


CreateWallet
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int CreateWallet(const std::string& seed, int coinType, const std::shared_ptr<BlockChainNode>& node, std::shared_ptr<HDWallet>* wallet)

  Create HD wallet.

  **Return**
    result of this function, 0 is succeded, < 0 is failed.

  **Parameter**
    :[in] seed: the seed.
    :[in] coinType: coin type, for example COIN_TYPE_ELA.
    :[in] node: the block chain node.
    :[out] wallet: shared ptr of HDWallet if succeeded, nullptr if failed.


CreateDidManager
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int CreateDidManager(const std::string& seed, std::shared_ptr<DidManager>* manager)

  Create DidManager.

  **Return**
    result of this function, 0 is succeded, < 0 is failed.

  **Parameter**
    :[in] seed: the seed.
    :[out] manager: shared ptr of DidManager if succeeded, nullptr if failed


HDWallet
================

.. cpp:class:: HDWallet

  Manage private key, address, create and send transaction, recover wallet.


GetCoinType
~~~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int GetCoinType()

  Get coin type of the wallet.

  **Return**
    Coin type of the wallet.


SendTransaction
~~~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int SendTransaction(const std::vector<Transaction>& transactions, const std::string& memo, const std::string& seed, std::string& txid, const std::string& chain = "")

  Send transaction.

  **Return**
    result of this function, 0 is succeded, < 0 is failed.

  **Parameter**
    :[in] transactions: the transactions.
    :[in] memo: memo of the transaction.
    :[in] seed: seed of the wallet.
    :[out] txid: the transaction id if succeeded.
    :[in] chain: the chain string for extension, default is empty string.


GetAddress
~~~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: std::string GetAddress(int chain, int index)

  Get address of wallet.

  **Return**
    The address.

    If it is single address wallet, returns the same address.

  **Parameter**
    :[in] chain: the chain code, EXTERNAL_CHAIN or INTERNAL_CHAIN.
    :[in] index: the index of the address.


GetPublicKey
~~~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: std::string GetPublicKey(int chain, int index)

  Get public key.

  **Return**
    The public key.

    If it is single address wallet, returns the same public key.

  **Parameter**
    :[in] chain: the chain code, EXTERNAL_CHAIN or INTERNAL_CHAIN.
    :[in] index: the index of the public key.


GetBalance
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: long GetBalance(const std::string& address)

  Get balance of the address.

  **Return**
    Balance in sela.

  **Parameter**
    :[in] address: the address.


GetBalance
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: long GetBalance()

  Get balance of the wallet.

  **Return**
    Balance in sela.


SyncHistory
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int SyncHistory()

  Synchronize history of the wallet from block chain node.

  Synchronized function, should be called in other thread.

  **Return**
    result of this function, 0 is succeded, < 0 is failed.


GetHistoryCount
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int GetHistoryCount(const std::string& address)

  Get history count of the address from local storage.

  **Return**
    history count.

  **Parameter**
    :[in] address: the address.


GetHistory
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int GetHistory(const std::string& address, int pageSize, int page, bool ascending, std::string& histories)

  Get history of the address from local storage.

  **Return**
    result of this function, 0 is succeded, < 0 is failed.

  **Parameter**
    :[in] address: the address.
    :[in] pageSize: size per page.
    :[in] page: page number, start from 0.
    :[in] ascending: order by time. true indicate ascending.
    :[out] histories: json string of histories.


GetUsedAddresses
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: std::vector<std::string> GetUsedAddresses()

  Get used addresses.

  **Return**
    Used addresses.


GetUnUsedAddresses
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: std::vector<std::string> GetUnUsedAddresses(unsigned int count)

  Get unused addresses.

  **Return**
    Unused addresses.

  **Parameter**
    :[in] count: unused address count expected.


Recover
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int Recover()

  Recover the wallet.

  Synchronized function, should be called in other thread.

  **Return**
    result of this function, 0 is succeded, < 0 is failed.


DidManager
================

.. cpp:class:: DidManager

  Create Did, recover Did.


CreateDid
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int CreateDid(int index, std::shared_ptr<Did>* did)

  Create Did Object.

  **Return**
    result of this function, 0 is succeded, < 0 is failed.

  **Parameter**
    :[in] index: the index of Did.
    :[in] did: shared ptr of Did if succeeded.


Recover
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: int Recover()

  Recover did.

  Synchronized function, should be called in other thread.

  **Return**
    result of this function, 0 is succeded, < 0 is failed.


Did
================

.. cpp:class:: Did

  Set/Get/Sign did info.


GetId
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: std::string GetId()

  Get the did, start with i.

  **Return**
    the did.


SignInfo
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: std::string SignInfo(const std::string& seed, const std::string& json)

  Sign did info.

  **Return**
    Signed data if succeeded, empty string if failed.

  **Parameter**
    :[in] seed: the seed.
    :[in] json: did info in json array string, for example: [{"Key":"name", "Value":"alice"}, {"Key":"age", "Value":18}].


SetInfo
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: std::string SetInfo(const std::string& seed, const std::string& json, const std::shared_ptr<HDWallet>& wallet)

  Set did info to id chain.

  **Return**
    Transaction id if succeeded, empty string if failed.

  **Parameter**
    :[in] seed: the seed.
    :[in] json: did info in json array string, for example: [{"Key":"name", "Value":"alice"}, {"Key":"age", "Value":18}].
    :[in] wallet: the id chain wallet to pay gas.


SyncInfo
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int SyncInfo()

  Synchronize did info from did service.

  **Return**
    result of this function, >= 0 is the count synced, < 0 is failed.


GetInfo
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: std::string GetInfo(const std::string& key)

  Get info of the key.

  **Return**
     info of the key in json string or empty string if not exist or the did property is deprecated.

  **Parameter**
    :[in] key: the key of the info.


BlockChainNode
================

.. cpp:class:: BlockChainNode

  Block chain node, manage url and certificate path.


BlockChainNode
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: BlockChainNode(const std::string& url)

  Constructor of the class.

  **Parameter**
      :[in] url: the url of the node.


SetUrl
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: void SetUrl(const std::string& url)

  Set ulr of the node.

  **Parameter**
      :[in] url: the url of the node.


GetUrl
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: std::string GetUrl()

  Get url of the node.

  **Return**
     Url of the node.


SetCertificate
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: void SetCertificate(const std::string& cert)

  Set cert file of the node.

  **Parameter**
      :[in] cert: local path of the certificate file.

.. warning:: this function is not impelemented yet.


Transaction
================

.. cpp:class:: Transaction

  Transaction Object.


Transaction
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: Transaction(const std::string& address, long amount, int coinType = COIN_TYPE_ELA)

  Constructor of the class.

  **Parameter**
      :[in] address: the address send to.
      :[in] amount: the amount to be sent in sela.
      :[in] coinType: the coin type of the address, default is COIN_TYPE_ELA.


SetAddress
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: void SetAddress(const std::string& address, int coinType)

  Set address of the transaction.

  **Parameter**
      :[in] address: the address send to.
      :[in] coinType: the coin type of the address.


SetAmount
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: void SetAmount(long amount)

  Set amount of the transaction.

  **Parameter**
      :[in] amount: the amount to be sent in sela.


GetCoinType
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int GetCoinType()

  Get coin type of the transaction.

  **Return**
     Coin type of the transaction.


GetAddress
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: std::string GetAddress()

  Get address of the transaction.

  **Return**
     Address of the transaction.


GetAmount
~~~~~~~~~~~~~~~~~~~~~

.. cpp:function:: long GetAmount()

  Get amount of the transaction.

  **Return**
     Amount of the transaction.

