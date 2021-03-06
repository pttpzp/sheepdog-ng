#ifndef TRACE_H
#define TRACE_H

#define INSN_SIZE       5       /* call(1b) + offset(4b) = 5b */

#ifndef __ASSEMBLY__

#include "sheep_priv.h"

struct caller {
	unsigned long addr;
	unsigned long fentry;
	const char *name;
	const char *section;
};

struct tracer {
	const char *name;

	void (*enter)(const struct caller *this_fn, int depth);
	void (*exit)(const struct caller *this_fn, int depth);

	/* internal use only */
	uatomic_bool enabled;
	int stack_depth;
	struct list_node list;
};

#define SD_MAX_STACK_DEPTH 1024

/* gcc defined*/
void __fentry__(void);

/* mcount.S */
void trace_caller(void);
void trace_return_caller(void);
void trace_function_enter(unsigned long, unsigned long *);
unsigned long trace_function_exit(void);

/* trace.c */
#ifdef HAVE_TRACE
  int trace_init(void);
  void regist_tracer(struct tracer *tracer);
  int trace_enable(const char *name);
  int trace_disable(const char *name);
  size_t trace_status(char *buf);
  int trace_buffer_pop(void *buf, uint32_t len);
  void trace_buffer_push(int cpuid, struct trace_graph_item *item);

#else
  static inline int trace_init(void) { return 0; }
  static inline int trace_enable(const char *name) { return 0; }
  static inline int trace_disable(const char *name) { return 0; }
  static inline size_t trace_status(char *buf) { return 0; }
  static inline int trace_buffer_pop(void *buf, uint32_t len) { return 0; }
  static inline void trace_buffer_push(
	  int cpuid, struct trace_graph_item *item) { return; }

#endif /* HAVE_TRACE */

#define tracer_register(tracer)			\
static void __attribute__((constructor))	\
regist_ ##tracer(void)				\
{						\
	regist_tracer(&tracer);			\
}

#endif /* __ASSEMBLY__ */
#endif
