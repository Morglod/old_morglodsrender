#pragma once

#ifndef _MR_FRAMEBUFFERS_CONFIG_H_
#define _MR_FRAMEBUFFERS_CONFIG_H_

#define MR_FRAMEBUFFERS_CHECK_BIND_ERRORS 1

#if (MR_FRAMEBUFFERS_CHECK_BIND_ERRORS == 1)
    #include "../Utils/Debug.hpp"
#endif

#if (MR_FRAMEBUFFERS_CHECK_BIND_ERRORS == 1)
    #define MR_FRAMEBUFFERS_CHECK_BIND_ERRORS_CATCH(x, y) CheckGLError(x, y)
#else
    #define MR_FRAMEBUFFERS_CHECK_BIND_ERRORS_CATCH(x, y)
#endif

#endif
