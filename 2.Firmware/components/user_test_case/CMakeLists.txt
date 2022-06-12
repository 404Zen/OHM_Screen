idf_component_register(SRCS "user_test_case.c"
                            "tc_parse_ohm.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES user_http_request
                    PRIV_REQUIRES ohm_data_parse 
                    PRIV_REQUIRES user_smartconfig
                    PRIV_REQUIRES encoder)  
