/**
 * @file sync.h
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 * @brief Low level storage synchronization routines
 */
/*
 * $Id: sync.h,v 2.1 2000/12/06 20:22:28 dulimart Exp $
 *
 * $Log: sync.h,v $
 * Revision 2.1  2000/12/06 20:22:28  dulimart
 * initial checkin
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef SYNC_H
#define SYNC_H
#ifdef __cplusplus
extern "C" {
#endif
void lock (int *);
void unlock (int * s);
#ifdef __cplusplus
}
#endif
#endif
