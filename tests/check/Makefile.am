include $(top_srcdir)/common/check.mak

TESTS_ENVIRONMENT = 

plugindir = $(libdir)/gstreamer-@GST_API_VERSION@

# override to _not_ install the test plugins
install-pluginLTLIBRARIES:

# the core dumps of some machines have PIDs appended
CLEANFILES = core.* test-registry.* *.gcno *.gcda

common_cflags=-I$(top_srcdir) $(GST_PLUGINS_BASE_CFLAGS) $(GST_OBJ_CFLAGS) \
	$(GST_CHECK_CFLAGS) $(GST_OPTION_CFLAGS) $(GST_CFLAGS)
common_ldadd=$(top_builddir)/ges/libges-@GST_API_VERSION@.la \
	$(GST_PLUGINS_BASE_LIBS) -lgstpbutils-$(GST_API_VERSION) \
	$(GST_OBJ_LIBS) $(GST_CHECK_LIBS)

testutils_noisnt_libraries=libtestutils.la
testutils_noinst_headers=ges/test-utils.h
libtestutils_la_LIBADD=$(common_ldadd)
libtestutils_la_CFLAGS=$(common_cflags)
libtestutils_la_SOURCES=ges/test-utils.c

SUPPRESSIONS = $(top_srcdir)/common/gst.supp # $(srcdir)/gst-plugins-bad.supp


clean-local: clean-local-check

check_PROGRAMS = \
	ges/backgroundsource\
	ges/basic	\
	ges/layer	\
	ges/effects	\
	ges/uriclip	\
	ges/clip	\
	ges/timelineedition	\
	ges/titles\
	ges/transition	\
	ges/overlays\
	ges/text_properties\
	ges/mixers\
	ges/group\
	ges/project

noinst_LTLIBRARIES=$(testutils_noisnt_libraries)
noinst_HEADERS=$(testutils_noinst_headers)

TESTS = $(check_PROGRAMS)

AM_CFLAGS =  $(common_cflags) -UG_DISABLE_ASSERT -UG_DISABLE_CAST_CHECKS
LDADD = $(common_ldadd) libtestutils.la

noinst_PROGRAMS = integration

integration_SOURCES = ges/integration.c
integration_LDADD = $(LDADD)
integration_CFLAGS = $(AM_CFLAGS)

EXTRA_DIST = \
	ges/test-project.xges \
	ges/test-auto-transition.xges \
	ges/test-keyframes.xges \
	ges/audio_only.ogg \
	ges/image.png \
	ges/audio_video.ogg

COVERAGE_DIRS = \
	ges
COVERAGE_FILES = $(foreach dir,$(COVERAGE_DIRS),$(wildcard $(top_builddir)/$(dir)/*.gcov))
COVERAGE_FILES_REL = $(subst $(top_builddir)/,,$(COVERAGE_FILES))
COVERAGE_OUT_FILES = $(foreach dir,$(COVERAGE_DIRS),$(wildcard $(top_builddir)/$(dir)/*.gcov.out))
COVERAGE_OUT_FILES_REL = $(subst $(top_builddir)/,,$(COVERAGE_OUT_FILES))

debug:
	echo $(COVERAGE_FILES)
	echo $(COVERAGE_FILES_REL)

.PHONY: coverage
if GST_GCOV_ENABLED
# we rebuild a registry and do gst-inspect so that all the get/set codepaths
# are also covered
coverage:
	make check
	make coverage-report
else
coverage:
	echo "You need to configure with --enable-gcov to get coverage data"
	exit 1
endif

coverage-report:
	if test ! -e coverage; then
		rm -r coverage
	fi
	for dir in $(COVERAGE_DIRS); do 				\
	  mkdir -p coverage/$$dir; 					\
	  make -C $(top_builddir)/$$dir gcov;				\
        done
	for dir in $(COVERAGE_DIRS); do					\
	    files="`ls $(top_builddir)/$$dir/*.gcov.out 2> /dev/null`"; \
          if test ! -z "$$files"; then					\
	    perl $(top_srcdir)/common/coverage/coverage-report.pl 	\
	      $(top_builddir)/$$dir/*.gcov.out >			\
	      coverage/$$dir/index.xml;					\
	    xsltproc $(top_srcdir)/common/coverage/coverage-report.xsl	\
	      coverage/$$dir/index.xml > coverage/$$dir/index.html;	\
	  fi; \
        done
	for file in $(COVERAGE_FILES_REL); do				\
	  echo Generating coverage/$$file.html; 			\
	  perl $(top_srcdir)/common/coverage/coverage-report-entry.pl   \
	    $(top_builddir)/$$file > coverage/$$file.html; 		\
	done

check-integration: integration
	CK_DEFAULT_TIMEOUT=20 ./integration

check-integration-forever:
	@while true; do						\
	make check-integration					\
	CK_DEFAULT_TIMEOUT=20					\
	$* || break; done

check-integration-gdb:
	@$(TESTS_ENVIRONMENT)					\
	CK_FORK=no						\
	$(LIBTOOL) --mode=execute				\
	gdb ./integration
