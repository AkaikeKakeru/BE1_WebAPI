#include <iostream>

#include <cpprest/filestream.h>
#include <cpprest/http_client.h>

using namespace utility;                // 文字列変換などの一般的なユーティリティ
using namespace web;                    // URIのような共通の機能
using namespace web::http;              // 共通のHTTP機能
using namespace web::http::client;      // HTTP クライアントの機能
using namespace concurrency::streams;   // 非同期ストリーム

template <class T>

pplx::task<T> GetAllFaculties(const std::wstring& url) {
	return pplx::create_task([=] {
		// クライアントの設定
		http_client client(url);

		// リクエスト送信
		return client.request(methods::GET);
		})
		.then([](http_response response) {
			// ステータスコート判定
			if (response.status_code() == status_codes::OK) {
				return response.extract_json();
			}
			});
}

pplx::task<int> Post(const std::wstring& url) {
	return pplx::create_task([=] {
		// 送信データの作成
		json::value postData;
		postData[L"name"] = json::value::string(L"ハイパーL4科");

		// クライアントの設定
		http_client client(url);
		// リクエスト送信
		return client.request(methods::POST, L"", postData.serialize(),
			L"application/json");
		})
		.then([](http_response response) {
			// ステータスコート判定
			if (response.status_code() == status_codes::Created) {
				// レスポンスボディを表示
				return response.extract_json();
			}
			})
			.then([](json::value json) {
				return json[L"rowCount"].as_integer();
				});
}

int main() {
	setlocale(LC_CTYPE, "ja_JP.UTF-8");
	try {
		//Get
		concurrency::task<web::json::value> task =
			GetAllFaculties<json::value>(L"http://localhost:3000/faculties/");
		const json::value j = task.get();
		web::json::array array = j.as_array();
		for (int i = 0; i < j.size(); i++) {
			std::wcout << array[i].at(U("name")) << std::endl;
		}
	}
	catch (const std::exception& e) {
		printf("Error exception:%s\n", e.what());
	}

	try {
		//Post
		auto response =
			Post(L"http://localhost:3000/faculties/").get();

		if (response == 1) {
			std::cout << "新規データを追加しました。" << std::endl;
		}
	}
	catch (const std::exception& e) {
		printf("Error exception:%s\n", e.what());
	}

	return 0;
}
