#include "global.h"
#include "CatalogXml.h"
#include "SongManager.h"
#include "RageLog.h"
#include "song.h"
#include "Steps.h"
#include "XmlFile.h"
#include "Course.h"
#include "SongUtil.h"
#include "StepsUtil.h"
#include "CourseUtil.h"
#include "TrailUtil.h"
#include "GameState.h"
#include <set>
#include "Foreach.h"


void SaveCatalogXml()
{
	CString fn = CATALOG_XML_FILE;

	LOG->Trace( "Writing %s ...", fn.c_str() );

	XNode xml;
	xml.name = "Catalog";

	vector<Song*> vpSongs = SONGMAN->GetAllSongs();
	for( unsigned i=0; i<vpSongs.size(); i++ )
	{
		Song* pSong = vpSongs[i];

		SongID songID;
		songID.FromSong( pSong );

		XNode* pSongNode = songID.CreateNode();

		xml.AppendChild( pSongNode );

		pSongNode->AppendChild( "MainTitle", pSong->GetDisplayMainTitle() );
		pSongNode->AppendChild( "SubTitle", pSong->GetDisplaySubTitle() );

		vector<Steps*> vpSteps = pSong->GetAllSteps();
		for( unsigned j=0; j<vpSteps.size(); j++ )
		{
			Steps* pSteps = vpSteps[j];
		
			if( pSteps->IsAutogen() )
				continue;

			StepsID stepsID;
			stepsID.FromSteps( pSteps );

			XNode* pStepsIDNode = stepsID.CreateNode();
			pSongNode->AppendChild( pStepsIDNode );
			
			pStepsIDNode->AppendChild( "Meter", pSteps->GetMeter() );
			pStepsIDNode->AppendChild( pSteps->GetRadarValues().CreateNode() );
		}
	}


	vector<Course*> vpCourses;
	SONGMAN->GetAllCourses( vpCourses, false );
	for( unsigned i=0; i<vpCourses.size(); i++ )
	{
		Course* pCourse = vpCourses[i];

		CourseID courseID;
		courseID.FromCourse( pCourse );

		XNode* pCourseNode = courseID.CreateNode();

		xml.AppendChild( pCourseNode );

		pCourseNode->AppendChild( "MainTitle", pCourse->GetDisplayMainTitle() );
		pCourseNode->AppendChild( "SubTitle", pCourse->GetDisplaySubTitle() );
		pCourseNode->AppendChild( "HasMods", pCourse->HasMods() );

		FOREACH_StepsType( st )
		{
			FOREACH_CourseDifficulty( cd )
			{
				Trail *pTrail = pCourse->GetTrail( st, cd );
				if( pTrail == NULL )
					continue;
				if( !pTrail->m_vEntries.size() )
					continue;
				
				TrailID trailID;
				trailID.FromTrail( pTrail );

				XNode* pTrailIDNode = trailID.CreateNode();
				pCourseNode->AppendChild( pTrailIDNode );
				
				pTrailIDNode->AppendChild( "Meter", pTrail->GetMeter() );
				pTrailIDNode->AppendChild( pTrail->GetRadarValues().CreateNode() );
			}
		}
	}

	{
		XNode* pNode = xml.AppendChild( "DifficultiesToShow" );

		set<Difficulty> vDiffs;
		GAMESTATE->GetDifficultiesToShow( vDiffs );
		for( set<Difficulty>::const_iterator iter = vDiffs.begin(); iter != vDiffs.end(); iter++ )
		{
			pNode->AppendChild( "Difficulty", DifficultyToString(*iter) );
		}
	}

	{
		XNode* pNode = xml.AppendChild( "CourseDifficultiesToShow" );

		set<CourseDifficulty> vDiffs;
		GAMESTATE->GetCourseDifficultiesToShow( vDiffs );
		for( set<CourseDifficulty>::const_iterator iter = vDiffs.begin(); iter != vDiffs.end(); iter++ )
		{
			pNode->AppendChild( "CourseDifficulty", DifficultyToString(*iter) );
		}
	}


	xml.SaveToFile(fn);

	LOG->Trace( "Done." );
}

/*
 * (c) 2004 Chris Danford
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
