
#include "Transaction.h"
#include "BlockChainNode.h"
#include "WalletError.h"
#include "common/Utils.h"
#include <vector>
#include "Identity.h"
#include "IdentityManager.h"
#include "Did.h"
#include "DidManager.h"
#include "MultiSignWallet.h"

using namespace elastos;

void TestSingleWallet();
void TestHDWallet();
void TestMultiSignWallet();

int main(int argc, char** argv)
{
    // TestSingleWallet();
    // TestHDWallet();
    TestMultiSignWallet();
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
    const char* path = "/Users/huahua/repo/Elastos.SDK.Keypair.C/src/Data/mnemonic_chinese.txt";
    char* words = readMnemonicFile(path);
    if (!words) {
        printf("read file failed\n");
        return;
    }

    std::string seed = IdentityManager::GetSeed(mnemonic, "chinese", words, "");
    printf("seed:%s\n", seed.c_str());

    std::shared_ptr<Identity> identity;
    ret = IdentityManager::CreateIdentity("/Users/huahua/repo/Elastos.SDK.Wallet.C", &identity);
    printf("identity ret:%d\n", ret);


    std::shared_ptr<BlockChainNode> node1 = std::make_shared<BlockChainNode>(TEST_NET_WALLET_SERVICE_URL);
    std::shared_ptr<HDWallet> hdWallet;
    ret = identity->CreateSingleAddressWallet(seed, node1, &hdWallet);
    printf("hd wallet ret:%d\n", ret);

    std::string publicKey = hdWallet->GetPublicKey(0, 0);
    std::string hdSingleAddress = hdWallet->GetAddress(0, 0);
    printf("hd address:%s\n", hdSingleAddress.c_str());

    long balance = hdWallet->GetBalance(hdSingleAddress);
    printf("balance: %ld\n", balance);

    // hdWallet->SyncHistory();

    std::shared_ptr<BlockChainNode> node = std::make_shared<BlockChainNode>(TEST_NET_DID_SERVICE_URL);
    std::shared_ptr<HDWallet> idChainWallet;
    ret = identity->CreateSingleAddressWallet(seed, node, &idChainWallet);

    balance = idChainWallet->GetBalance(hdSingleAddress);
    printf("id chain balance: %ld\n", balance);

    idChainWallet->SyncHistory();


    // std::vector<Transaction> transactions;
    // Transaction tx1("EdVgb5RTdmwKf79pEUdVNnFprWyvmr1hPc", 100000000L);
    // transactions.push_back(tx1);
    // Transaction tx2("ETajy5FmdzhngRUJmwKJFJBMv8EM8BhXnH", 100000000L);
    // transactions.push_back(tx2);
    // Transaction tx3("ENvWkDwYdt1m5F2Pi7WEVDXk89T8qne6jj", 50000000L);
    // transactions.push_back(tx3);
    // Transaction tx4("Ed2656EoB37fr2DSnqbLvp8G5cxDtZKYf4", 20000000L);
    // transactions.push_back(tx4);
    // Transaction tx5("EUFDLS15BFZHMGkRHTwM6Sz4gYuoejXrHZ", 10000000L);
    // transactions.push_back(tx5);

    // std::string txid;
    // ret = hdWallet->SendTransaction(transactions, "", seed, txid);
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
    ret = manager->CreateDid(0, &didObj);
    std::string did = didObj->GetId();
    printf("did: %s\n", did.c_str());

    didObj->SetNode(node);

    std::string str("[{\"Key\": \"name\", \"Value\":\"alice\"}]");
    std::string info = didObj->SignInfo(seed, str);
    printf("signed info: %s\n", info.c_str());

    // std::string txid = didObj->SetInfo(seed, str, idChainWallet);
    // printf("did set info txid: %s\n", txid.c_str());

    didObj->SyncInfo();

    // std::string info = didObj->GetInfo("name");
    // printf("the did info: %s\n", info.c_str());

}

std::string GetAddress()
{
    const char* mnemonic = "督 辉 稿 谋 速 壁 阿 耗 瓷 仓 归 说";
    const char* path = "/Users/huahua/repo/Elastos.SDK.Keypair.C/src/Data/mnemonic_chinese.txt";
    char* words = readMnemonicFile(path);
    if (!words) {
        printf("read file failed\n");
        return "";
    }

    std::string seed = IdentityManager::GetSeed(mnemonic, "chinese", words, "");
    printf("seed:%s\n", seed.c_str());

    std::shared_ptr<Identity> identity;
    int ret = IdentityManager::CreateIdentity("/Users/huahua/repo/Elastos.SDK.Wallet.C", &identity);
    printf("identity ret:%d\n", ret);

    std::shared_ptr<BlockChainNode> node1 = std::make_shared<BlockChainNode>(TEST_NET_WALLET_SERVICE_URL);
    std::shared_ptr<HDWallet> hdWallet;
    ret = identity->CreateSingleAddressWallet(seed, node1, &hdWallet);
    printf("hd wallet ret:%d\n", ret);

    std::string publicKey = hdWallet->GetPublicKey(0, 0);
    std::string hdSingleAddress = hdWallet->GetAddress(0, 0);
    printf("hd address:%s\n", hdSingleAddress.c_str());

    hdWallet->SyncHistory();

    long balance = hdWallet->GetBalance(hdSingleAddress);
    printf("balance: %ld\n", balance);

    return hdSingleAddress;
}

void TestHDWallet()
{
    int ret;
    const char* mnemonic = "血 坝 告 售 代 讨 转 枝 欧 旦 诚 抱";
    const char* path = "/Users/huahua/repo/Elastos.SDK.Keypair.C/src/Data/mnemonic_chinese.txt";
    char* words = readMnemonicFile(path);
    if (!words) {
        printf("read file failed\n");
        return;
    }

    std::string seed = IdentityManager::GetSeed(mnemonic, "chinese", words, "");
    printf("seed:%s\n", seed.c_str());

    std::shared_ptr<Identity> identity;
    ret = IdentityManager::CreateIdentity("/Users/huahua/repo/Elastos.SDK.Wallet.C", &identity);
    printf("identity ret:%d\n", ret);

    std::shared_ptr<BlockChainNode> node1 = std::make_shared<BlockChainNode>(TEST_NET_WALLET_SERVICE_URL);
    std::shared_ptr<HDWallet> hdWallet;
    ret = identity->CreateWallet(seed, COIN_TYPE_ELA, node1, &hdWallet);
    printf("hd wallet ret:%d\n", ret);

    // hdWallet->SyncHistory();
    // hdWallet->Recover();

    int balance = hdWallet->GetBalance();
    printf("balance: %d\n", balance);

    // std::string addr = GetAddress();
    // printf("to addr: %s\n", addr.c_str());
    std::vector<Transaction> transactions;
    Transaction tx1("8SC6up2F6XFej1gzY1uuFrCxFevF5P5jFw", 10000000L);
    transactions.push_back(tx1);

    std::string txid;
    ret = hdWallet->SendTransaction(transactions, "", seed, txid);
    if (ret != E_WALLET_C_OK) {
        printf("send transaction failed: %d\n", ret);
        return;
    }
    printf("txid: %s\n", txid.c_str());

    // std::vector<std::string> usedAddrs = hdWallet->GetUsedAddresses();
    // printf("used address:\n");
    // for (std::string addr : usedAddrs) {
    //     printf("%s\n", addr.c_str());
    // }
}

void TestMultiSignWallet()
{
    std::vector<std::string> publicKeys;
    publicKeys.push_back("031ed85c1a56e912de5562657c6d6a03cfe974aab8b62d484cea7f090dac9ff1cf");
    publicKeys.push_back("0306ee2fa3fb66e21b61ac1af9ce95271d9bb5fc902f92bd9ff6333bda552ebc64");
    publicKeys.push_back("03b8d95fa2a863dcbd44bf288040df4c6cb9d674a61c4c1e3638ac515994c777e5");

    const char* mnemonic = "血 坝 告 售 代 讨 转 枝 欧 旦 诚 抱";
    const char* path = "/Users/huahua/repo/Elastos.SDK.Keypair.C/src/Data/mnemonic_chinese.txt";
    char* words = readMnemonicFile(path);
    if (!words) {
        printf("read file failed\n");
        return;
    }

    std::string seed = IdentityManager::GetSeed(mnemonic, "chinese", words, "");
    printf("seed:%s\n", seed.c_str());

    std::shared_ptr<Identity> identity;
    int ret = IdentityManager::CreateIdentity("/Users/huahua/repo/Elastos.SDK.Wallet.C", &identity);
    printf("identity ret:%d\n", ret);

    std::shared_ptr<BlockChainNode> node = std::make_shared<BlockChainNode>(TEST_NET_WALLET_SERVICE_URL);
    std::shared_ptr<MultiSignWallet> wallet;
    ret = identity->CreateWallet(publicKeys, 2, COIN_TYPE_ELA, node, &wallet);
    printf("multi sign wallet ret:%d\n", ret);

    std::string address = wallet->GetAddress();
    printf("multi sign wallet addr: %s\n", address.c_str());

    // ret = wallet->SyncHistory();
    // printf("SyncHistory ret:%d\n", ret);

    int balance = wallet->GetBalance();
    printf("balance: %d\n", balance);

    std::string addr = GetAddress();
    printf("to addr: %s\n", addr.c_str());
    std::vector<Transaction> transactions;
    Transaction tx1(addr, 1000000L);
    transactions.push_back(tx1);

    std::string json = wallet->SignTransaction(transactions, seed, 0, 1);
    printf("first sign: %s\n", json.c_str());

    json = wallet->SignTransaction(json, seed, 0, 2);
    printf("second sign: %s\n", json.c_str());

    std::vector<std::string> signedSigners = wallet->GetSignedSigners(json);
    for (int i = 0; i < signedSigners.size(); i++) {
        printf("signed: %s\n", signedSigners[i].c_str());
    }

    // std::string txid;
    // ret = wallet->SendTransaction(json, txid);
    // if (ret != E_WALLET_C_OK) {
    //     printf("send transaction failed: %d\n", ret);
    //     return;
    // }
    // printf("txid: %s\n", txid.c_str());
}

