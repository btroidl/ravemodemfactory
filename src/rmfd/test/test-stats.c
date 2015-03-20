/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 * librmfd-stats tests
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 * Copyright (C) 2015 Zodiac Inflight Innovations
 *
 * Author: Aleksander Morgado <aleksander@aleksander.es>
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <rmfd-stats.h>
#include <rmfd-syslog.h>

void
rmfd_syslog (gint type, const gchar *fmt, ...)
{
    /* Do nothing */
}

static void
common_test (const gchar *contents,
             const gchar *expected_contents,
             guint        expected_year,
             guint        expected_month,
             guint64      expected_rx_bytes,
             guint64      expected_tx_bytes)
{
    RmfdStatsContext *ctx;
    gchar            *path  = NULL;
    GError           *error = NULL;
    gint              handle;
    gchar            *new_contents = NULL;

    handle = g_file_open_tmp ("rmfd-stats-XXXXXX", &path, &error);
    g_assert_no_error (error);
    g_assert (handle > 0);
    g_assert (write (handle, contents, strlen (contents)) > 0);
    close (handle);

    ctx = rmfd_stats_setup (path);

    g_assert_cmpuint (expected_year,     ==, rmfd_stats_get_year     (ctx));
    g_assert_cmpuint (expected_month,    ==, rmfd_stats_get_month    (ctx));
    g_assert_cmpuint (expected_rx_bytes, ==, rmfd_stats_get_rx_bytes (ctx));
    g_assert_cmpuint (expected_tx_bytes, ==, rmfd_stats_get_tx_bytes (ctx));

    g_assert (g_file_get_contents (path, &new_contents, NULL, &error));
    g_assert_no_error (error);
    g_assert_cmpstr (expected_contents, ==, new_contents);
    g_free (new_contents);

    rmfd_stats_teardown (ctx);

    g_unlink (path);
    g_free (path);
}

static void
test_iso8601_single_full (void)
{
    const gchar *contents =
        "S" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n";

    common_test (contents, contents, 2015, 3, 329880, 80021);
}

static void
test_iso8601_single_partial (void)
{
    const gchar *contents =
        "S" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n";

    const gchar *expected_contents =
        "S" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n";

    common_test (contents, expected_contents, 2015, 3, 329880, 80021);
}

static void
test_iso8601_multiple_full (void)
{
    const gchar *contents =
        "S" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n";

    common_test (contents, contents, 2015, 3, 3 * 329880, 3 * 80021);
}

static void
test_iso8601_multiple_partial (void)
{
    const gchar *contents =
        "S" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n";

    const gchar *expected_contents =
        "S" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n";

    common_test (contents, expected_contents, 2015, 3, 3 * 329880, 3 * 80021);
}

static void
test_iso8601_multiple_partial_last_invalid (void)
{
    const gchar *contents =
        "S" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:50" "\t" "41" "\t" "329";

    const gchar *expected_contents =
        "S" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "2015-03-19 11:53:09" "\t" "2015-03-19 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "2015-03-20 11:53:09" "\t" "2015-03-20 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:09" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:20" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:30" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "2015-03-21 11:53:09" "\t" "2015-03-21 11:53:40" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n";

    common_test (contents, expected_contents, 2015, 3, 3 * 329880, 3 * 80021);
}

static void
test_unix_single_full (void)
{
    const gchar *contents =
        "S" "\t" "(unix) 1426860384" "\t" "1426860384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "(unix) 1426860384" "\t" "1426860415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n";

    common_test (contents, contents, 2015, 3, 329880, 80021);
}

static void
test_unix_single_partial (void)
{
    const gchar *contents =
        "S" "\t" "(unix) 1426860384" "\t" "1426860384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n";

    const gchar *expected_contents =
        "S" "\t" "(unix) 1426860384" "\t" "1426860384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "(unix) 1426860384" "\t" "1426860415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n";

    common_test (contents, expected_contents, 2015, 3, 329880, 80021);
}

static void
test_unix_multiple_full (void)
{
    const gchar *contents =
        "S" "\t" "(unix) 1426860384" "\t" "1426860384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "(unix) 1426860384" "\t" "1426860415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "(unix) 1426861384" "\t" "1426861384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426861384" "\t" "1426861395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426861384" "\t" "1426861405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "(unix) 1426861384" "\t" "1426861415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "(unix) 1426862384" "\t" "1426862384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426862384" "\t" "1426862395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426862384" "\t" "1426862405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "(unix) 1426862384" "\t" "1426862415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n";

    common_test (contents, contents, 2015, 3, 3 * 329880, 3 * 80021);
}

static void
test_unix_multiple_partial (void)
{
    const gchar *contents =
        "S" "\t" "(unix) 1426860384" "\t" "1426860384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "(unix) 1426860384" "\t" "1426860415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "(unix) 1426861384" "\t" "1426861384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426861384" "\t" "1426861395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426861384" "\t" "1426861405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "(unix) 1426861384" "\t" "1426861415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "(unix) 1426862384" "\t" "1426862384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426862384" "\t" "1426862395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426862384" "\t" "1426862405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426862384" "\t" "1426862415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n";

    const gchar *expected_contents =
        "S" "\t" "(unix) 1426860384" "\t" "1426860384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "(unix) 1426860384" "\t" "1426860415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "(unix) 1426861384" "\t" "1426861384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426861384" "\t" "1426861395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426861384" "\t" "1426861405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "(unix) 1426861384" "\t" "1426861415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "(unix) 1426862384" "\t" "1426862384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426862384" "\t" "1426862395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426862384" "\t" "1426862405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "(unix) 1426862384" "\t" "1426862415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n";

    common_test (contents, expected_contents, 2015, 3, 3 * 329880, 3 * 80021);
}

static void
test_unix_multiple_partial_last_invalid (void)
{
    const gchar *contents =
        "S" "\t" "(unix) 1426860384" "\t" "1426860384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "(unix) 1426860384" "\t" "1426860415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "(unix) 1426861384" "\t" "1426861384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426861384" "\t" "1426861395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426861384" "\t" "1426861405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "(unix) 1426861384" "\t" "1426861415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "(unix) 1426862384" "\t" "1426862384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426862384" "\t" "1426862395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426862384" "\t" "1426862405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426862384" "\t" "1426862415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426862384" "\t" "1426862425" "\t" "31" "\t" "329";

    const gchar *expected_contents =
        "S" "\t" "(unix) 1426860384" "\t" "1426860384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426860384" "\t" "1426860405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "(unix) 1426860384" "\t" "1426860415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "(unix) 1426861384" "\t" "1426861384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426861384" "\t" "1426861395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426861384" "\t" "1426861405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "(unix) 1426861384" "\t" "1426861415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"

        "S" "\t" "(unix) 1426862384" "\t" "1426862384" "\t" "0"  "\t" "0"       "\t" "0"      "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426862384" "\t" "1426862395" "\t" "11" "\t" "244686"  "\t" "46346"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "P" "\t" "(unix) 1426862384" "\t" "1426862405" "\t" "21" "\t" "305946"  "\t" "65562"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n"
        "F" "\t" "(unix) 1426862384" "\t" "1426862415" "\t" "31" "\t" "329880"  "\t" "80021"  "\t" "umts" "\t" "-93" "\t" "214" "\t" "3" "\t" "1140" "\t" "10774738" "\n";

    common_test (contents, expected_contents, 2015, 3, 3 * 329880, 3 * 80021);
}

int main (int argc, char **argv)
{
    g_test_init (&argc, &argv, NULL);

    g_test_add_func ("/librmfd-stats/iso8601/single/full",                   test_iso8601_single_full);
    g_test_add_func ("/librmfd-stats/iso8601/single/partial",                test_iso8601_single_partial);
    g_test_add_func ("/librmfd-stats/iso8601/multiple/full",                 test_iso8601_multiple_full);
    g_test_add_func ("/librmfd-stats/iso8601/multiple/partial",              test_iso8601_multiple_partial);
    g_test_add_func ("/librmfd-stats/iso8601/multiple/partial/last-invalid", test_iso8601_multiple_partial_last_invalid);

    g_test_add_func ("/librmfd-stats/unix/single/full",                      test_unix_single_full);
    g_test_add_func ("/librmfd-stats/unix/single/partial",                   test_unix_single_partial);
    g_test_add_func ("/librmfd-stats/unix/multiple/full",                    test_unix_multiple_full);
    g_test_add_func ("/librmfd-stats/unix/multiple/partial",                 test_unix_multiple_partial);
    g_test_add_func ("/librmfd-stats/unix/multiple/partial/last-invalid",    test_unix_multiple_partial_last_invalid);

    return g_test_run ();
}