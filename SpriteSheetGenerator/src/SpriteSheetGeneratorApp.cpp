#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "pockets/ImagePacker.h"
#include "cinder/ip/Premultiply.h"
#include "cinder/ip/Resize.h"
#include "cinder/svg/Svg.h"
#include "cinder/cairo/Cairo.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace pockets;

/**
SpriteSheetGenerator for WordShift game
*/

struct FontData
{
  int tile_font_size;
  int heading_font_size;
  int heading_large_size;
  Vec2f tile_font_offset;
  FontData operator / (const int divisor) const
  {
    FontData ret;
    ret.tile_font_size = tile_font_size / divisor;
    ret.tile_font_offset = tile_font_offset / divisor;
    ret.heading_font_size = heading_font_size / divisor;
    ret.heading_large_size = heading_large_size / divisor;
    return ret;
  }
};

class SpriteSheetGeneratorApp : public AppNative
{
public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

  //! make a sprite sheet with given font data and list of files
  void    createSheet( const string &name, const FontData &font_data, const string &layer_name, float svg_scale );
  //! resize/rename/(premultiply) graphic and place in project resources
  void    prepareGraphic( const fs::path &source, const std::string &output_name );
  //! rename and png-format the given graphic; for batching PSD output
  void    copyGraphic( const fs::path &source, const std::string &output_name );
  Surface renderSvgToSurface( const svg::Node &node, float scale );
  void    extractColors( const svg::DocRef &doc, const std::string &label_id, float scale, pk::ImagePacker *packer );
  void    assignRegistration( ImagePacker::ImageDataRef image, const svg::Node *node, float scale );
  fs::path getOutputPath();

private:
  svg::DocRef mIconDoc;
  svg::DocRef mColorDoc;
};

void SpriteSheetGeneratorApp::setup()
{
  mIconDoc = svg::DocRef( new svg::Doc( loadAsset( "icons.svg" ) ) );
  mColorDoc = svg::Doc::create( getAssetPath( "colors.svg" ) );

  FontData retinaIpad;
  retinaIpad.tile_font_size = 96;
  retinaIpad.tile_font_offset = Vec2f( 0, -6 );
  retinaIpad.heading_font_size = 48;
  retinaIpad.heading_large_size = 60;
  createSheet( "sprite_sheet_ipad@2x", retinaIpad, "ipad-retina", 1.0f );
  createSheet( "sprite_sheet_ipad", retinaIpad / 2, "ipad-retina", 0.5f );

  FontData retinaIphone;
  retinaIphone.tile_font_size = 72;
  retinaIphone.tile_font_offset = Vec2f( 0, -2 );
  retinaIphone.heading_font_size = 32;
  retinaIphone.heading_large_size = 36;
  createSheet( "sprite_sheet_iphone@2x", retinaIphone, "iphone-retina", 1.0f );
  createSheet( "sprite_sheet_iphone", retinaIphone / 2, "iphone-retina", 0.5f );

  copyGraphic( getAssetPath("../../Design/warnings/countdown-3.psd"), "countdown-3.png" );
  copyGraphic( getAssetPath("../../Design/warnings/countdown-2.psd"), "countdown-2.png" );
  copyGraphic( getAssetPath("../../Design/warnings/countdown-1.psd"), "countdown-1.png" );
  copyGraphic( getAssetPath("../../Design/warnings/countdown-0.psd"), "countdown-0.png" );

  quit();
}

void SpriteSheetGeneratorApp::extractColors(const svg::DocRef &doc, const std::string &label_id, float scale, pk::ImagePacker *image_packer)
{
  ci::JsonTree json = JsonTree::makeArray("palettes");

  const svg::Group *palettes = doc->findByIdContains<svg::Group>( "palettes" );
  for( auto child : palettes->getChildren() )
  {
    svg::Group *group = dynamic_cast<svg::Group*>( child );
    if( group && !group->getChildren().empty() )
    {
      string group_id = split( group->getId(), "_" ).at( 0 );
      JsonTree palette;
      palette.pushBack( JsonTree( "name", group_id ) );
      cout << "Color Palette: " << group_id << endl;
      for( auto node : group->getChildren() )
      {
        string id = split( node->getId(), "_" ).at( 0 );
        if( id != label_id )
        {
          ColorA8u color = node->getFill().getColor();
          JsonTree colorJson = JsonTree::makeObject( id );
          colorJson.pushBack( JsonTree("r", int(color.r)) );
          colorJson.pushBack( JsonTree("g", int(color.g)) );
          colorJson.pushBack( JsonTree("b", int(color.b)) );
          palette.pushBack( colorJson );
        }
      }
      json.pushBack( palette );

      auto label_node = group->findNodeByIdContains( label_id );
      if( label_node )
      {
        Surface label = renderSvgToSurface( *label_node, scale );
        auto image = image_packer->addImage( "cp-" + group_id + "-label", label );
        assignRegistration( image, label_node, scale );
      }
      else
      {
        cout << "No label found for " << group_id << endl;
      }
    }
  }
  ci::JsonTree container;
  container.pushBack( json );
  container.write( getOutputPath() / "img/colors.json" );
}

void SpriteSheetGeneratorApp::createSheet( const string &name, const FontData &font_data, const string &layer_name, float svg_scale )
{
  pk::ImagePacker image_packer;
//  image_packer.setWidth( 2048 );
  cout << "Building sheet: " << name << endl;
  // Create main tile text sprites
  const string font_name = "Avenir Next Bold";
  Font font( font_name, font_data.tile_font_size );
  auto glyphs = image_packer.addGlyphs( font, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "", false );
  for( auto glyph : glyphs )
  {
    glyph->setRegistrationPoint( glyph->getSize() / 2 + font_data.tile_font_offset );
  }
  // Add wildcard sprite (2x size asterisk)
  /*
  Font large_font( font_name, font_data.tile_font_size * 2 );
  auto asterisk = image_packer.addString( "*", large_font, "*", true );
  asterisk->setRegistrationPoint( asterisk->getSize() / 2 );
  */
  // Add numerical sprites for UI
  Font end_state_font( "Avenir Next Demi Bold", font_data.heading_font_size );
  Font end_large_font( "Avenir Next Demi Bold", font_data.heading_large_size );
  image_packer.addGlyphs( end_state_font, "0123456789", "", false );
  image_packer.addGlyphs( end_large_font, "0123456789", "g", false );

  // load SVGs and turn into sprites
  const svg::Group *group = mIconDoc->find<svg::Group>( layer_name );
  for( svg::Node *child : group->getChildren() )
  {
    string id = split( child->getId(), "_" ).at( 0 );
    cout << id << endl;
    auto image = image_packer.addImage( id, renderSvgToSurface( *child, svg_scale ) );
    assignRegistration( image, child, svg_scale );
  }

  // get color SVGs
  extractColors( mColorDoc, "label-" + layer_name, svg_scale, &image_packer );

  // Save
  fs::path output_path = getOutputPath();
  auto file = writeFile( output_path / "sprites" / (name + ".json") );
  if( file )
  {
    image_packer.calculatePositions( Vec2i( 2048, 2 ) );
    image_packer.surfaceDescription().write( file );
    writeImage( output_path / "sprites" / (name + ".png"), image_packer.packedSurface() );
  }

  cout << "Finished\n" << endl;
}

Surface SpriteSheetGeneratorApp::renderSvgToSurface(const svg::Node &node, float scale)
{
  svg::Paint white( ColorA8u( 255, 255, 255, 255 ) );
  auto visitor = [=](const svg::Node& n, svg::Style *style)->bool
                  { // render everything except registration marks
                    if( n.getId().find("registration-mark") != string::npos ){ return false; }
                    if( n.getId().find("boundary-shape") != string::npos ){ return false; }
                    // fill rules are not applied to paths, because they are strokes
                    // text boxes will become solid white with a fill rule
                    // will successfully affect groups.
                    style->setFill( white );
                    return true;
                  };
  Rectf rect = node.getBoundingBox();
  cairo::SurfaceImage srfImg( rect.getWidth() * scale, rect.getHeight() * scale, true );
	cairo::Context ctx( srfImg );
  ctx.scale( scale, scale );
  ctx.translate( -(rect.getUpperLeft()) );
	ctx.render( node, visitor );
  Surface ret = srfImg.getSurface();
  ip::unpremultiply( &ret );
  return ret;
}

void SpriteSheetGeneratorApp::assignRegistration(ImagePacker::ImageDataRef image, const svg::Node *node, float scale )
{
  const svg::Group *group = dynamic_cast<const svg::Group*>( node );
  if( group )
  {
    auto registration_mark = group->findNodeByIdContains( "registration-mark" );
    if( registration_mark )
    {
      image->setRegistrationPoint( (registration_mark->getBoundingBox().getCenter() - group->getBoundingBox().getUpperLeft()) * scale );
    }
  }
}

void SpriteSheetGeneratorApp::prepareGraphic(const fs::path &source, const std::string &output_name)
{
  Surface full_image = loadImage( source );
  fs::path out = getOutputPath();
  writeImage( out / "img" / (output_name + "@2x.png"), full_image );

  // scale down and save again
  ip::premultiply( &full_image );
  Surface half_image( full_image.getWidth() / 2, full_image.getHeight() / 2, full_image.hasAlpha(), full_image.getChannelOrder() );
  ip::resize( full_image, &half_image );
  ip::unpremultiply( &half_image );
  writeImage( out / "img" / (output_name + ".png"), half_image );
}

void SpriteSheetGeneratorApp::copyGraphic(const fs::path &source, const std::string &output_name)
{
  Surface full_image = loadImage( source );
  fs::path out = getOutputPath();
  writeImage( out / "img" / output_name, full_image );
}

fs::path SpriteSheetGeneratorApp::getOutputPath()
{
  fs::path output_path = getAppPath().parent_path();
  if( output_path.stem() == "Debug" || output_path.stem() == "Release" )
  { // running from inside XCode or similar, so back up a bunch of directories
    output_path = output_path / "../../../../WordShift/resources";
  }
  return output_path;
}

void SpriteSheetGeneratorApp::mouseDown( MouseEvent event )
{
}

void SpriteSheetGeneratorApp::update()
{
}

void SpriteSheetGeneratorApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( SpriteSheetGeneratorApp, RendererGl )
