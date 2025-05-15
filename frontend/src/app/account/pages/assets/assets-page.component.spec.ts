import { ComponentFixture, TestBed } from '@angular/core/testing';
import { AssetsPage } from './assets-page.component';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';

describe('AssetsPage', () => {
  let component: AssetsPage;
  let fixture: ComponentFixture<AssetsPage>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [AssetsPage],
      imports: [MatProgressSpinnerModule],
    }).compileComponents();
  });

  function setupFixture() {
    fixture = TestBed.createComponent(AssetsPage);
    component = fixture.componentInstance;
    fixture.detectChanges();
  }

  it('should create the component', () => {
    const okResponse = new Response(JSON.stringify({}), { status: 200, statusText: 'OK' });
    spyOn(window, 'fetch').and.resolveTo(okResponse);

    setupFixture();

    expect(component).toBeTruthy();
  });
});
