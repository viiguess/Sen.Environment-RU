#pragma once

#include "kernel/interface/shell.hpp"

namespace Sen::Kernel::Interface::Script {

	/**
	 * ----------------------------------------
	 * JS namespace is used
	 * ----------------------------------------
	*/

	namespace JS = Sen::Kernel::Definition::JavaScript;

	/**
	 * ----------------------------------------
	 * JS Method Declaration
	 * ----------------------------------------
	*/

	typedef JSValue (*JavaScriptNativeMethod)(JSContext *, JSValueConst, int, JSValueConst *);

	/**
	 * JavaScript Console Interactive
	*/

	namespace Console {
		/**
		 * ----------------------------------------
		 * JS Print method
		 * @param argv[0]: title
		 * @param argv[1]: color
		 * ----------------------------------------
		*/

		inline static auto print(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 2 or argc == 1, fmt::format("argument expected {} but received {}", "2 or 1", argc));
			auto str = JS_ToCString(context, argv[0]);
			if(argc == 2){
				Shell{}.print(JS::Converter::get_string(context, argv[0]).c_str(),static_cast<Sen::Kernel::Interface::Color>(JS::Converter::get_int32(context, argv[1])));
			}
			else{
				Shell{}.print(JS::Converter::get_string(context, argv[0]).c_str(), Sen::Kernel::Interface::Color::DEFAULT);
			}
			JS_FreeCString(context, str);
			return JS::Converter::get_undefined();
		}

		/**
		 * ----------------------------------------
		 * JS Readline method
		 * @returns: input data
		 * ----------------------------------------
		*/

		inline static auto readline(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 0, fmt::format("argument expected {} but received {}", 0, argc));
			auto result = Shell{}.input();
			auto m_result = std::string{result.data, result.size};
			return JS::Converter::to_string(context, m_result);
		}
	}

	/**
	 * JavaScript File System
	*/

	namespace FileSystem {

		/**
		 * ----------------------------------------
		 * JS read file method
		 * @param argv[0]: source file
		 * @return: readed file
		 * ----------------------------------------
		*/

		inline static auto read_file(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto result = Sen::Kernel::FileSystem::readFile(source);
			bool c = JS::Converter::get_bool(context, argv[1]);
			JS_FreeCString(context, source);
			return JS::Converter::to_string(context, result);
		}

		/**
		 * ----------------------------------------
		 * JS write file method
		 * @param argv[0]: destination file
		 * @param argv[1]: data to write
		 * @return: written file
		 * ----------------------------------------
		*/

		inline static auto write_file(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
			auto destination = JS_ToCString(context, argv[0]);
			auto data = JS_ToCString(context, argv[1]);
			Sen::Kernel::FileSystem::writeFile(destination, data);
			JS_FreeCString(context, destination);
			JS_FreeCString(context, data);
			return JS::Converter::get_undefined();
		}
	}

	/**
	 * JavaScript Engine
	*/

	namespace JavaScript {
		/**
		 * ----------------------------------------
		 * JavaScript evaluate method
		 * @param argv[0]: JS code to execute
		 * @return: JS value after evaluate
		 * ----------------------------------------
		*/

		inline static auto evaluate(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto m_value = JS_Eval(context, source, strlen(source), "unknown", JS_EVAL_TYPE_GLOBAL);
			JS_FreeCString(context, source);
			return m_value;
		}

		/**
		 * ----------------------------------------
		 * JavaScript evaluate as file
		 * @param argv[0]: source file
		 * @return: JS Value after evaluate
		 * ----------------------------------------
		*/

		inline static auto evaluate_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto js_source = Sen::Kernel::FileSystem::readFile(source);
			auto m_value = JS_Eval(context, js_source.c_str(), js_source.size(), source, JS_EVAL_TYPE_GLOBAL);
			JS_FreeCString(context, source);
			return m_value;
		}
	}

	/**
	 * JavaScript Encryption System
	*/

	namespace Encryption {

		/**
		 * JavaScript Encryption System Supportive of MD5
		*/

		namespace MD5 {
			/**
			 * ----------------------------------------
			 * JavaScript MD5 Hash method
			 * @param argv[0]: source hash
			 * @returns: hashed string
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto str = std::string{source};
				auto result = Kernel::Definition::Encryption::MD5::hash(static_cast<std::span<unsigned char>>(String::convertStringToSpan<unsigned char>(str)));
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript MD5 Hash file method
			 * @param argv[0]: source file
			 * @returns: hashed string
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::MD5::hash_fs(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}
		}

		/**
		 * JavaScript Base64 Encode/Decode System
		*/

		namespace Base64 {

			/**
			 * ----------------------------------------
			 * JavaScript Base 64 Encode method
			 * @param argv[0]: source to encode
			 * @returns: Encoded string
			 * ----------------------------------------
			*/

			inline static auto encode(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::Base64::encode(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript Base 64 Decode method
			 * @param argv[0]: source to decode
			 * @returns: Decoded string
			 * ----------------------------------------
			*/

			inline static auto decode(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::Base64::decode(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript Base 64 Encode method
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto encode_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				Sen::Kernel::Definition::Encryption::Base64::encode_fs(source, destination);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}

			/**
			 * ----------------------------------------
			 * JavaScript Base 64 Encode method
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto decode_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				Sen::Kernel::Definition::Encryption::Base64::decode_fs(source, destination);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}
		}

		/**
		 * JavaScript SHA224 Hash 
		*/

		namespace SHA224 {
			/**
			 * ----------------------------------------
			 * JavaScript SHA224 Hash
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::Sha224::hash(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript SHA224 Hash File System
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::Sha224::hash_fs(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

		}

		/**
		 * JavaScript SHA256 Support
		*/

		namespace SHA256 {
			/**
			 * ----------------------------------------
			 * JavaScript SHA256 Hash
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::SHA256::hash(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript SHA256 Hash File System
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::SHA256::hash_fs(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}
		}

		/**
		 * JavaScript SHA384 hash Support
		*/

		namespace SHA384 {
			/**
			 * ----------------------------------------
			 * JavaScript SHA384 Hash
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::SHA384::hash(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript SHA384 Hash File System
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::SHA384::hash_fs(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}
		}

		/**
		 * JavaScript SHA512 hash Support
		*/

		namespace SHA512 {
			/**
			 * ----------------------------------------
			 * JavaScript SHA512 Hash
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::SHA512::hash(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript SHA512 Hash File System
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto result = Sen::Kernel::Definition::Encryption::SHA512::hash_fs(source);
				JS_FreeCString(context, source);
				return JS::Converter::to_string(context, result);
			}
		}

		/**
		 * JavaScript XOR Encryption
		*/

		namespace XOR {
			/**
			 * ----------------------------------------
			 * JavaScript XOR Encryption
			 * @param argv[0]: key
			 * @param argv[1]: plain
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto encrypt(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto plain = JS_ToCString(context, argv[0]);
				auto key = JS_ToCString(context, argv[1]);
				auto result = Sen::Kernel::Definition::Encryption::XOR::encrypt(plain, key);
				JS_FreeCString(context, plain);
				JS_FreeCString(context, key);
				return JS::Converter::to_string(context, result);
			}

			/**
			 * ----------------------------------------
			 * JavaScript XOR Encryption File System
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @param argv[2]: key
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto encrypt_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				auto key = JS_ToCString(context, argv[2]);
				Sen::Kernel::Definition::Encryption::XOR::encrypt_fs(source, destination, key);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				JS_FreeCString(context, key);
				return JS::Converter::get_undefined();
			}

		}
	}

	/**
	 * JavaScript Compression Support
	*/

	namespace Compression {

		/**
		 * JavaScript Zlib Support
		*/

		namespace Zlib {
			/**
			 * ----------------------------------------
			 * JavaScript Zlib Compression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @param argv[2]: level
			 * @returns: Compressed file
			 * ----------------------------------------
			*/

			inline static auto compress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				auto level = JS::Converter::get_int32(context, argv[2]);
				if(!(Sen::Kernel::Definition::Compression::Zlib::Level::DEFAULT <= level or level <= Sen::Kernel::Definition::Compression::Zlib::Level::LEVEL_9)){
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					throw std::invalid_argument(fmt::format("Invalid zlib level, expected level from 0 to 9, received {}", level));
				}
				Sen::Kernel::Definition::Compression::Zlib::compress_fs(source, destination, static_cast<Sen::Kernel::Definition::Compression::Zlib::Level>(level));
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}

			/**
			 * ----------------------------------------
			 * JavaScript Zlib Uncompression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @returns: Uncompressed file
			 * ----------------------------------------
			*/

			inline static auto uncompress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				Sen::Kernel::Definition::Compression::Zlib::uncompress_fs(source, destination);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}
			
		}

		/**
		 * JavaScript Gzip Compression Support
		*/

		namespace Gzip {
			/**
			 * ----------------------------------------
			 * JavaScript Gzip Compression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @returns: Compressed file
			 * ----------------------------------------
			*/

			inline static auto compress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				Sen::Kernel::Definition::Compression::Zlib::compress_gzip_fs(source, destination);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}

			/**
			 * ----------------------------------------
			 * JavaScript Gzip Uncompression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @returns: Uncompressed file
			 * ----------------------------------------
			*/

			inline static auto uncompress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
				auto source = JS_ToCString(context, argv[0]);
				auto destination = JS_ToCString(context, argv[1]);
				Sen::Kernel::Definition::Compression::Zlib::uncompress_gzip_fs(source, destination);
				JS_FreeCString(context, source);
				JS_FreeCString(context, destination);
				return JS::Converter::get_undefined();
			}
		}
	}

	namespace Support {

		namespace PopCap {
			/**
			 * JavaScript RTON Support
			*/
			
			namespace RTON {
				/**
				 * ----------------------------------------
				 * JavaScript RTON Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Uncompressed file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					Sen::Kernel::Support::PopCap::RTON::Decode::decode_fs(source, destination);
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript RTON Encode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Uncompressed file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}
			}

			/**
			 * JavaScript RSB Support
			*/

			namespace RSB {
				/**
				 * ----------------------------------------
				 * JavaScript RSB Unpack File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Uncompressed file
				 * ----------------------------------------
				*/

				inline static auto unpack_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript RSB Pack File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Uncompressed file
				 * ----------------------------------------
				*/

				inline static auto pack_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}
			}

			/**
			 * JavaScript RSG Support
			*/

			namespace RSG {
				/**
				 * ----------------------------------------
				 * JavaScript RSG Unpack File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Uncompressed file
				 * ----------------------------------------
				*/

				inline static auto unpack_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript RSG Pack File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Uncompressed file
				 * ----------------------------------------
				*/

				inline static auto pack_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}
			}

			/**
			 * JavaScript PAM Supportive
			*/

			namespace Animation {
				/**
				 * ----------------------------------------
				 * JavaScript PAM Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Uncompressed file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript PAM Encode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Uncompressed file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}
			}
		}

		/**
		 * JavaScript WWise Support
		*/

		namespace WWise {

			/**
			 * JavaScript SoundBank Support
			*/

			namespace SoundBank {
				/**
				 * ----------------------------------------
				 * JavaScript WWise Soundbank Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Uncompressed file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}

				/**
				 * ----------------------------------------
				 * JavaScript WWise Soundbank Encode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Uncompressed file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
					auto source = JS_ToCString(context, argv[0]);
					auto destination = JS_ToCString(context, argv[1]);
					// encode method
					JS_FreeCString(context, source);
					JS_FreeCString(context, destination);
					return JS::Converter::get_undefined();
				}
			}
		}
	}

	/**
	 * JavaScript Custom JSON Serializer
	*/

	namespace JSON {

		/**
		 * Convert nlohmann::json to JSValue for quickjs
		*/

		inline static auto json_to_js_value(
			JSContext *context, 
			const nlohmann::json &json
		) -> JSValue
		{
			switch (json.type()) {
				case nlohmann::json::value_t::object: {
					auto js_obj = JS_NewObject(context);
					for (auto &[key, value] : json.items()) {
						JS_DefinePropertyValueStr(context, js_obj, key.c_str(), json_to_js_value(context, value), JS_PROP_C_W_E);
					}
					return js_obj;
				}
				case nlohmann::json::value_t::array: {
					auto js_arr = JS_NewArray(context);
					for (auto i : Range<size_t>(json.size())) {
						JS_DefinePropertyValueUint32(context, js_arr, i, json_to_js_value(context, json[i]), JS_PROP_C_W_E);
					}
					return js_arr;
				}
				case nlohmann::json::value_t::string:{
					return JS_NewString(context, json.get<std::string>().c_str());
				}
				case nlohmann::json::value_t::boolean:{
					return JS_NewBool(context, json.get<bool>());
				}
				case nlohmann::json::value_t::number_integer:{
					return JS_NewBigInt64(context, json.get<int64_t>());
				}
				case nlohmann::json::value_t::number_unsigned:{
					return JS_NewBigInt64(context, json.get<uint64_t>());
				}
				case nlohmann::json::value_t::number_float:{
					return JS_NewFloat64(context, json.get<double>());
				}
				default:{
					return JS::Converter::get_undefined();
				}
			}
		}
		
		/**
		 * ----------------------------------------
		 * JavaScript JSON Deserializer
		 * @param argv[0]: JS String
		 * @returns: Deserialized object
		 * ----------------------------------------
		*/

		inline static auto deserialize(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
			auto source = JS_ToCString(context, argv[0]);
			auto json = nlohmann::json::parse(source);
			auto js_obj = json_to_js_value(context, json);
			JS_FreeCString(context, source);
			return js_obj;
		}

		/**
		 * QuickJS JSON Value to nlohmann::json and then to string
		*/

		auto js_object_to_json(
			JSContext *context, 
			JSValueConst value
		) -> nlohmann::json
		{
			switch (JS_VALUE_GET_TAG(value)) {
				case JS_TAG_OBJECT: {
					if (JS_IsArray(context, value)) {
						auto json = nlohmann::json::array();
						auto length = uint32_t{};
						JS_ToUint32(context, &length, JS_GetPropertyStr(context, value, "length"));
						for (auto i : Range<uint32_t>(length)) {
							auto val = JS_GetPropertyUint32(context, value, i);
							json.push_back(js_object_to_json(context, val));
							JS_FreeValue(context, val);
						}
						return json;
					} 
					else if (JS_IsObject(value)) {
						auto json = nlohmann::json::object();
						auto *tab = static_cast<JSPropertyEnum*>(nullptr);
						auto tab_size = uint32_t{};
						if (JS_GetOwnPropertyNames(context, &tab, &tab_size, value, JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY) == 0) {
							for (auto i : Range<uint32_t>(tab_size)) {
								auto key = JS_AtomToCString(context, tab[i].atom);
								auto val = JS_GetProperty(context, value, tab[i].atom);
								json[key] = js_object_to_json(context, val);
								JS_FreeAtom(context, tab[i].atom);
								JS_FreeValue(context, val);
							}
							js_free(context, tab);
						}
						return json;
					}
					else{
						throw std::runtime_error("Unknown type");
					}
				}
				case JS_TAG_STRING: {
					auto str = JS_ToCString(context, value);
					auto json = nlohmann::json(str);
					JS_FreeCString(context, str);
					return json;
				}
				case JS_TAG_BOOL:{
					return nlohmann::json(JS_VALUE_GET_BOOL(value));
				}
				case JS_TAG_INT:{
					return nlohmann::json((double) JS_VALUE_GET_INT(value));
				}
				case JS_TAG_FLOAT64:{
					return nlohmann::json(JS_VALUE_GET_FLOAT64(value));
				}
				case JS_TAG_BIG_INT: {
					auto val = int64_t{};
					JS_ToBigInt64(context, &val, value);
					return nlohmann::json(val);
				}
				default:{
					return nullptr;
				}
			}
		}

		/**
		 * ----------------------------------------
		 * JavaScript JSON Serializer
		 * @param argv[0]: JSValue
		 * @param argv[1]: indent
		 * @param argv[2]: char
		 * @param argv[3]: ensure ascii
		 * @returns: Serialized object
		 * ----------------------------------------
		*/

		inline static auto serialize(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
			auto json = js_object_to_json(context, argv[0]);
			auto indent = JS::Converter::get_int32(context, argv[1]);
			auto ensure_ascii = JS::Converter::get_bool(context, argv[2]);;
			auto source = json.dump(indent, '\t', ensure_ascii);
			auto js_val = JS_NewString(context, source.c_str());
			return js_val;
		}
	}

}