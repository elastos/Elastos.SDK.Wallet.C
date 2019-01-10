
#include "Transaction.h"
#include "BlockChainNode.h"
#include "WalletError.h"
#include "Elastos.Wallet.Utility.h"
#include "common/Utils.h"
#include <vector>
#include "Identity.h"
#include "IdentityManager.h"
#include "Did.h"
#include "DidManager.h"

using namespace elastos;

void TestSingleWallet();

int main(int argc, char** argv)
{
    TestSingleWallet();
    return 0;
}

char* readMnemonicFile(const char* path)
{
    FILE* file = fopen(path, "r");
    if (!file) {
        return nullptr;
    }
    char* buf = (char*)malloc(1024 * 10);
    if (!buf) {
        fclose(file);
        return nullptr;
    }
    int count = 0;
    char c;

    while ( (c = fgetc(file)) != EOF) {
        buf[count++] = c;
    }

    buf[count] = '\0';
    fclose(file);

    return buf;
}

void TestSingleWallet()
{
    int ret;
    const char* mnemonic = "血 坝 告 售 代 讨 转 枝 欧 旦 诚 抱";
    const char* path = "/Users/nathansfile/Elastos.ORG.Wallet.Lib.C/src/Data/mnemonic_chinese.txt";
    char* words = readMnemonicFile(path);
    if (!words) {
        printf("read file failed\n");
        return;
    }

    std::string seed = IdentityManager::GetSeed(mnemonic, "chinese", words, "");
    printf("seed:%s\n", seed.c_str());

    std::shared_ptr<Identity> identity;
    int index = IdentityManager::CreateIdentity("/Users/nathansfile/Elastos.SDK.Wallet.C", &identity);
    printf("identity index:%d\n", index);


    std::unique_ptr<BlockChainNode> node1 = std::make_unique<BlockChainNode>(TEST_NET_WALLET_SERVICE_URL);
    std::shared_ptr<HDWallet> hdWallet;
    index = identity->CreateSingleAddressWallet(seed, node1, &hdWallet);
    printf("hd wallet index:%d\n", index);

    std::string publicKey = hdWallet->GetPublicKey(0, 0);
    std::string hdSingleAddress = hdWallet->GetAddress(0, 0);
    printf("hd address:%s\n", hdSingleAddress.c_str());

    long balance = hdWallet->GetBalance(hdSingleAddress);
    printf("balance: %ld\n", balance);

    // hdWallet->SyncHistory();

    std::unique_ptr<BlockChainNode> node = std::make_unique<BlockChainNode>(TEST_NET_DID_SERVICE_URL);
    std::shared_ptr<HDWallet> idChainWallet;
    index = identity->CreateSingleAddressWallet(seed, node, &idChainWallet);

    balance = idChainWallet->GetBalance(hdSingleAddress);
    printf("id chain balance: %ld\n", balance);

    idChainWallet->SyncHistory();


    // Transaction tx(hdSingleAddress, 100000000L);
    // std::vector<Transaction> transactions;
    // transactions.push_back(tx);
    // std::string txid;
    // ret = hdWallet->SendTransaction(transactions, "cross chain transaction", seed, txid, "idchain");
    // if (ret != E_WALLET_C_OK) {
    //     printf("send transaction failed: %d\n", ret);
    //     return;
    // }

    // printf("txid: %s\n", txid.c_str());


    std::string histories;
    int count = hdWallet->GetHistoryCount(hdSingleAddress);
    hdWallet->GetHistory(hdSingleAddress, 5, 0, false, histories);
    printf("history: %s\n", histories.c_str());

    // balance = wallet->GetBalance();
    // printf("balance: %ld\n", balance);

    std::shared_ptr<DidManager> manager;
    ret = identity->CreateDidManager(seed, &manager);
    if (ret != 0) {
        printf("create Did manager failed\n");
        return;
    }

    std::shared_ptr<Did> didObj;
    index = manager->CreateDid(0, &didObj);
    std::string did = didObj->GetId();
    printf("did: %s\n", did.c_str());

    // std::string str("[{\"Key\": \"name\", \"Value\":\"alice\"}]");
    // std::string info = didObj->SignInfo(seed, str);
    // printf("signed info: %s\n", info.c_str());

    // std::string txid = didObj->SetInfo(seed, str, idChainWallet);
    // printf("did set info txid: %s\n", txid.c_str());

    didObj->SyncInfo();

    std::string info = didObj->GetInfo("name");
    printf("the did info: %s\n", info.c_str());

}
