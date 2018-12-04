#ifndef _HTTP_CLIENT_HPP_
#define _HTTP_CLIENT_HPP_

#include <curl/curl.h>
#include <map>
#include <string>

namespace elastos {

class HttpClient {
public:
  /*** type define ***/
	class ErrCode {
	public:
		static constexpr int Unknown        = -1;
		static constexpr int NotFound       = -2;
		static constexpr int NullArgument   = -3;
		static constexpr int BadArgument    = -4;
		static constexpr int ExceptionThrew = -5;
		static constexpr int UserCanceled   = -6;
		static constexpr int IOFailed       = -7;
		static constexpr int NetFailed      = -8;

		static const char* ToString(int errcode);
	private:
		explicit ErrCode() = delete;
		virtual ~ErrCode() = delete;
	};

  /*** static function and variable ***/

  /*** class function and variable ***/
	explicit HttpClient();
	virtual ~HttpClient();

	int Url(const std::string& url);
	int AddHeader(const std::string& name, const std::string& value);
	int SetTimeout(long milliSecond);

	int Get();
	int Post(void* body);

	int GetResponseStatus();
	int GetResponseReason(std::string& message);
	int GetResponseHeaders(std::map<std::string, std::string>& headers);
	int GetResponseHeader(const std::string& name, std::string& value);
  int GetResponseBody(std::shared_ptr<void*>& body);

protected:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/

private:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/
	std::string mUrl;
	std::map<std::string, std::string> mHeader;
	long mTimeoutMS;
	int mRespStatus;
	int mRespReason;
	std::shared_ptr<void*> mRespBody;
};

/***********************************************/
/***** class template function implement *******/
/***********************************************/

/***********************************************/
/***** macro definition ************************/
/***********************************************/

} // namespace elastos

#endif /* _HTTP_CLIENT_HPP_ */

