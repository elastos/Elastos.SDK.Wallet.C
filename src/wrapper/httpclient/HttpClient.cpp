#include "HttpClient.hpp"

namespace elastos {

/* =========================================== */
/* === static variables initialize =========== */
/* =========================================== */


/* =========================================== */
/* === static function implement ============= */
/* =========================================== */

/* =========================================== */
/* === class public function implement  ====== */
/* =========================================== */
HttpClient::HttpClient()
	: mUrl()
	, mHeader()
	, mTimeoutMS(10000)
	, mRespStatus(0)
	, mRespReason()
	, mRespBody(nullptr)
{
}

HttpClient::~HttpClient()
{
}

int HttpClient::Url(const std::string& url)
{
	if(url.empty() == true) {
		return ErrCode::NullArgument;
	}

	const std::string schemeHttp = "http://";
	const std::string schemeHttps = "https://";
	if(url.compare(0, schemeHttp.length(), schemeHttp) != 0
	&& url.compare(0, schemeHttps.length(), schemeHttp) != 0) {
		return ErrCode::BadArgument;
	}

	mUrl = url;
	return 0;
}

/* =========================================== */
/* === class protected function implement  === */
/* =========================================== */


/* =========================================== */
/* === class private function implement  ===== */
/* =========================================== */

} // namespace elastos
